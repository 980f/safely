#ifndef DIN_H
#define DIN_H  (C) 2017, Andrew L. Heilveil

#include "gpio.h"

#include <serialdevice.h>

/** a digital input can be either a GPIO or a serial port handshake line.
 created so that a USB to serial adaptor can be used in place of raspberry pi's builtin port.*/
struct Din {
  GPIO *bygp;
  SerialDevice::Pin *bysp;
  ~Din();
public:
  Din();

  GPIO *beGpio(unsigned pinIndex,unsigned af,unsigned pull);//make a gpio
  SerialDevice::Pin *beSpio(SerialDevice &port,SerialDevice::Pin::Which one, bool invert);//make an spio

  void connect(SerialDevice *port, unsigned which, bool invert=true);
public:
  bool readpin() const noexcept;
  operator bool() noexcept ;
};

#endif // DIN_H
