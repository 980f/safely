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


void Din::connect(SerialDevice *port, unsigned which, bool invert){
  if(port!=nullptr) {
    beSpio(*port,spinMap[which%countof(spinMap)],invert);
  } else {
    beGpio(which,0,0);//maydo: use invert to control pull direction?
  }
}

bool Din::readpin() const noexcept{
    if(bygp) {
      return bygp->readpin();
    }
    if(bysp) {
      return *bysp;//todo:0 check that this reads the actual pin
    }
    return false;
}


Din::operator bool() noexcept {
  return readpin();
}


