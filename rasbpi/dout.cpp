#include "dout.h"

Dout::~Dout(){
  Free(&bygp);
  Free(&bysp);
}

Dout::Dout(){
  bygp = nullptr;
  bysp = nullptr;
}

GPIO* Dout::beGpio(unsigned pinIndex, unsigned af, unsigned pull){
  return bygp = new GPIO(pinIndex,af,pull);
}

SerialDevice::Pin *Dout::beSpio(SerialDevice &port, SerialDevice::Pin::Which one, bool invert){
  return bysp = port.makePin(one,invert);
}

void Dout::connect(SerialDevice *port,unsigned which,bool initValue){
  if(port!=nullptr) {
    beSpio(*port,which?SerialDevice::Pin::Rts:SerialDevice::Pin::Dtr,1);//lastarg: host logic tends to polarity
  } else {
    beGpio(which,1,0);       //simple output
  }
  operator = (initValue);//set initial value
}

void Dout::operator =(bool value) noexcept {
  if(bygp) {
    *bygp = value;
  }
  if(bysp) {
    *bysp = value;
  }
}

Dout::operator bool() noexcept {
  if(bygp) {
    return *bygp;
  }
  if(bysp) {
    return *bysp;
  }
  return false;
}

void Dout::toggle() noexcept {
  if(bygp) {
    bygp->toggle();
  }
  if(bysp) {
    bysp->toggle();
  }
}
