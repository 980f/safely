#include "pwm.h"

#include "bitbanger.h"
#include "clockgenerator.h"

#include "continuedfractionratiogenerator.h"

static class PwmControl:public Peripheral {
  enum PwmControlBit{
    Enable ,
    Serialize,
    RepeatFifoLast, //when true and sending fifo data then repeats last entry when fifo is empty
    EmptyState,//output when fifo empty and repeatFifoLast is false
    LowActive,//xor'd with logical output value
    Fifoing, //use fifo content else data register is numerator
    ClearFifo, //command bit, apparently only used with first pwm
    PatternGenerator,//output pattern else simple pwm.
  };

public:
  PwmControl():Peripheral( 0x20C000,0x28),
    zero(0),
    one(1){
    //don't touch anything quite yet.
  }

  void configureForSimple(bool which){
    //todo: use bitField classes to implement 2nd pwm
    reg[0]=which?0:0;
  }

  void enable(bool which, bool onElseOff){
    assignBit(reg[0],Enable+(which?8:0),onElseOff);
  }

  void setPolarity(bool which, bool lowActive){
    assignBit(reg[0],LowActive+(which?8:0),lowActive);
  }

  void setRatio(bool which, unsigned leading, unsigned total){
    reg[4+(which<<4)]=total;
    reg[5+(which<<4)]=leading;
  }

  /** allocate all possible ones*/
  Pwm zero;
  Pwm one;

} ctl;

Pwm::Pwm(bool which) : pwm1(which){
  //done
}


Pwm *Pwm::get(bool which){
  return which?&ctl.one:&ctl.zero;
}

void Pwm::configure(bool lowActive){
  ctl.configureForSimple(pwm1);
  ctl.setPolarity(pwm1,lowActive);
}

void Pwm::setRatio(unsigned leading, unsigned total){
  ctl.setRatio(pwm1,leading,total);
}

void Pwm::setDutyCycle(double fraction){
  ContinuedFractionRatioGenerator generator(fraction,bitMask(0,10));
  setRatio(generator.numerator(),generator.denominator());
}

void Pwm::enable(bool onElseOff) const {
  ctl.enable(pwm1,onElseOff);
}

void Pwm::setSharedClock(double hz){
  ClockGenerator::module.setFreq(ClockGenerator::For::pwm,hz);
}


#if 0

output can either be simple pwm logic or a cyclic emission of either the data register or the fifo.
In other words there are two peripherals here: a pwm and a pattern generator.
The pattern generator has either an up to 32 bit pattern or you can feed it a pattern via a fifo which in turn is often fed by dma.

As a pwm the frequency is updated each cycle from the fifo if it is enabled

There is only one FIFO. It is shared by both channels.
The fifo reset is embedded in the first channels controls.

There are controls for 4 channels but only two are implemented.


#endif
