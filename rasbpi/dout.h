#ifndef DOUT_H
#define DOUT_H  "(C) 2017, Andrew L. Heilveil"

#include "gpio.h"

#include <serialdevice.h>

/** a digital out can be either a GPIO or a serial port handshake line.
 created so that a USB to serial adaptor can be used in place of raspberry pi's builtin port.*/
struct Dout {
  GPIO *bygp;
  SerialDevice::Pin *bysp;
  ~Dout();
public:
  Dout();

  GPIO *beGpio(unsigned pinIndex,unsigned af,unsigned pull);//make a gpio
  SerialDevice::Pin *beSpio(SerialDevice &port,SerialDevice::Pin::Which one, bool invert);//make an spio

  void connect(SerialDevice *port, unsigned which, bool initValue);

public: // AbstractPin interface
  void operator =(bool value) noexcept ;
  operator bool() noexcept ;
  void toggle() noexcept ;
};
#endif // DOUT_H
