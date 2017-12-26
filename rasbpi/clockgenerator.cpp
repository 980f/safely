#include "clockgenerator.h"
/*

 #define CLK_OSC_FREQ   19200000
 #define CLK_PLLD_FREQ 500000000
 *  reg[]:
 #define CLK_GP0_CTL 28
 #define CLK_GP0_DIV 29
 *
 #define CLK_GP1_CTL 30
 #define CLK_GP1_DIV 31
 *
 #define CLK_GP2_CTL 32
 #define CLK_GP2_DIV 33
 *
 #define CLK_PCMCTL 38
 #define CLK_PCMDIV 39
 *
 #define CLK_PWMCTL 40
 #define CLK_PWMDIV 41
 */

ClockGenerator ClockGenerator::module;

enum ClockBit : unsigned {
  BUSY=(1u << 7),
  KILL=(1u << 5),
  ENAB=(1u << 4)
};

ClockGenerator::ClockGenerator() :
  Peripheral(0x00101000,42){//2nd value is CLKJ_PWMDIV+1

}

unsigned ClockGenerator::init(For which,const Setting &setting){
  volatile unsigned *control= &reg[ which * 2]; //each clock control block is 2 words long
  volatile unsigned *divider= control + 1;
  volatile unsigned membarrier;
  /* pigpio kills the clock if busy, claims anything else isn't reliable, that contradicts the manual which says use Enab and wait patiently */
  do {
    *control= BCM_PASSWD & ~ClockBit::ENAB; //pigpio uses | KILL which the manual says can lock up the target of the clock.
    membarrier = *control;
  } while (membarrier & ClockBit::BUSY);

  unsigned ctlbase = BCM_PASSWD | (setting.MASH << 9) | (setting.usePLLD ? 6 : 1);
  *control = ctlbase;
  *divider = BCM_PASSWD | (setting.divI << 12) | setting.divF;
  membarrier = *divider + *control; //trying to force writing to divider before setting control bit.
  *control = ctlbase | ClockBit::ENAB;
  return membarrier;//just for emphasis on not optimizing out the delays this variable entails.
//and clock might not change frequency for a few cpu cycles, so set your clock frequency as the first step in programming a peripheral.
} // ClockGenerator::init

bool ClockGenerator::setFreq(For which,double hz){
  if(hz<=0.0) {
    return false;
  }
  Setting pwm;
  pwm.usePLLD = 0;//until we implement the required quards to limit to 25Mhz force use of slowest clock.
  double divider = ratio(19.2e6,hz);//19.2e6 is raspbi mode 1 clock.
  int whole = splitter(divider);
  if(whole>= (1 << 12)) {//12 bit field
    return false;
  }
  pwm.divI = unsigned(whole);

  pwm.divF = unsigned(divider * (1 << 10));//10 is what is given in other people's code, bsm manual suggests that 12 is the correct value.

  pwm.MASH = pwm.divF!=0 ? ClockGenerator::Setting::Dithering::Simple : ClockGenerator::Setting::Dithering::Off;

  init(which,pwm);
  return true;
} // ClockGenerator::setPwmFreq
