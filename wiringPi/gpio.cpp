#include "gpio.h"

#include "bitbanger.h"
#include "index.h"
const unsigned yesIreallyMeanIt=0x5A000000;


static bool fakeit=true;
//////////////
Mapped<unsigned> GPIO::gpioBase(fakeit?0:0x7e200000,40);//just big enough to get to pullup stuff, but not to test interface.

GPIO::GPIO(unsigned pinIndex):
  pinIndex(pinIndex),
  mask(1<<(pinIndex%32)),
  offset(pinIndex>=32) //will be zero or 1 for valid pinIndex
{

}

void GPIO::operator =(bool value) const noexcept{
  gpioBase[offset+(value?SetBits:ClearBits)]=mask;
}

GPIO::operator bool() const noexcept {
  return (gpioBase[offset+Read]&mask)!=0;
}

void GPIO::configure(unsigned af, unsigned pull){
  unsigned word=af/10;
  unsigned fielder=3*(af%10);
  mergeField(gpioBase[word],af,(fielder+3),fielder);
  if(pull!=0){
    //oh joy, inline delays of 150 clocks needed ....
    //todo: set puller register
    /*00 = Off – disable pull-up/down
    01 = Enable Pull Down control
    10 = Enable Pull Up control*/

    //wait 150, presumably at 150Mhz.
    //set clock bit to some unspecified value, pigpio says 0
    //wait 150
    //clr clock bit, which is not a documented thing, so they probably mean to pulse it again but wtf it really makes no sense.
    //writing a zero to the register, although they claim only writing a 1 does anything.
  }
}
