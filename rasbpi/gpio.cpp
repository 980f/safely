//"(C) Andrew L. Heilveil, 2017"
#include "gpio.h"

#include "bitbanger.h"
#include "index.h"

//////////////
Peripheral GPIO::base(0x200000,40);//just big enough to get to pullup stuff, but not to test interface.

GPIO::GPIO() :
  pinIndex(0),
  mask(0),
  offset(0){
  //#non yet usable
}

/** default for active level is that the pullup makes a signal inactive. For default of 'don't pull it' a legacy of high active is set */
GPIO::GPIO(unsigned pinIndex, unsigned af, int pull):active(pull<=0){
  connectTo(pinIndex);
  configure(af);
  pullit(pull);
}

GPIO &GPIO::connectTo(unsigned pinIndex){
  this->pinIndex = pinIndex;
  mask = (1 << (pinIndex % 32));
  offset = (pinIndex>=32); //will be zero or 1 for valid pinIndex
  return *this;
}

void GPIO::operator =(bool value) const noexcept {
  base.reg[offset + ((value==active) ? SetBits : ClearBits)] = mask;
}

bool GPIO::readpin() const noexcept {
  return active==((base.reg[offset + Read] & mask)!=0);
}

GPIO& GPIO::configure(unsigned af){
  unsigned word = pinIndex / 10; //10 pins per config register. 3 bits each.
  unsigned fielder = 3 * (pinIndex % 10);
  mergeField(base.reg[word],af,(fielder + 3),fielder);
  return *this;
}

GPIO& GPIO::pullit(int pull){
  //set puller register
  base.reg[PullerCode] = (pull>0) ? 2 : (pull<0) ? 1 : 0;
  //wait 150, presumably at 150Mhz.
  nanoSpin(150);
  //set clock bit to program above value into the selected bit(s)
  base.reg[offset + PullerClock] = mask;
  nanoSpin(150);
  //writing a zero to the register, although they claim only writing a 1 does anything everyone else seem to think this is needed:
  base.reg[offset + PullerClock] = 0; //todo:2 see if this is necessary
  return *this;
} // GPIO::pullit

GPIOpin::GPIOpin(GPIO &raw) : raw(raw){
  //#nada
}

void GPIOpin::operator =(bool value) noexcept {
  raw = value;
}

GPIOpin::operator bool() noexcept {
  return raw;
}

////
struct pwmPinMapping {
  unsigned gpio;
  unsigned altcode;
};

#if 0  //built the essential cases into a switch vs these arrays which can handle all cases.
static const pwmPinMapping pwm0pins[] = {
  {12,0},
  {18,5},
  //next not available on connector
  {40,0},
  {52,1},
  {0,0}
};

static const pwmPinMapping pwm1pins[] = {
  {13,0},
  {19,5},
  //next not available on connector
  {41,0},
  {45,0},
  {53,1},
  {0,0}
};
#endif
