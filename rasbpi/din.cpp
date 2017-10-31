#include "din.h"

Din::~Din(){
  Free(&bygp);
  Free(&bysp);
}

Din::Din(){
  bygp = nullptr;
  bysp = nullptr;
}

GPIO* Din::beGpio(unsigned pinIndex, unsigned af, unsigned pull){
  return bygp = new GPIO(pinIndex,af,pull);
}

SerialDevice::Pin *Din::beSpio(SerialDevice &port, SerialDevice::Pin::Which one, bool invert){
  return bysp = port.makePin(one,invert);
}

static SerialDevice::Pin::Which spinMap[]={
  SerialDevice::Pin::Dsr,
  SerialDevice::Pin::Cts,
  SerialDevice::Pin::Dcd,
  SerialDevice::Pin::Ri
};


void Din::connect(SerialDevice *port,unsigned which,bool initValue){
  if(port!=nullptr) {
    beSpio(*port,spinMap[which%countof(spinMap)],1);//lastarg: host logic tends to polarity
  } else {
    beGpio(which,1,0);       //simple output
  }
  operator = (initValue);//set initial value
}


Din::operator bool() noexcept {
  if(bygp) {
    return *bygp;
  }
  if(bysp) {
    return *bysp;
  }
  return false;
}


