//"(C) Andrew L. Heilveil, 2017"
#include "gpio.h"

#include "bitbanger.h"
#include "index.h"

//////////////
Mapped<unsigned> GPIO::gpioBase(0x7e200000,40);//just big enough to get to pullup stuff, but not to test interface.

GPIO::GPIO() :
  pinIndex(0),
  mask(0),
  offset(0){
  //#non usable
}

GPIO::GPIO(unsigned pinIndex, unsigned af, int pull){
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
  gpioBase[offset + (value ? SetBits : ClearBits)] = mask;
}


bool GPIO::readpin() const noexcept {
//  auto address= &gpioBase[offset + Read];
//  auto bitset=gpioBase[offset + Read];
//  auto forsure=*address;
//  if(forsure==bitset)
  return (gpioBase[offset + Read] & mask)!=0;
//  else return false;
}

GPIO& GPIO::configure(unsigned af){
  unsigned word = pinIndex / 10;
  unsigned fielder = 3 * (pinIndex % 10);
  mergeField(gpioBase[word],af,(fielder + 3),fielder);
  return *this;
}

GPIO& GPIO::pullit(int pull){
  //set puller register
  gpioBase[PullerCode] = (pull>0) ? 2 : (pull<0) ? 1 : 0;//
  //wait 150, presumably at 150Mhz.
  nanoSpin(150);
  //set clock bit
  gpioBase[offset + PullerClock] = mask;
  nanoSpin(150);
  //writing a zero to the register, although they claim only writing a 1 does anything.
  gpioBase[offset + PullerClock] = 0;
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
