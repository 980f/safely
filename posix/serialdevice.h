#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H "(C) Andrew L. Heilveil, 2017"

#include "fildes.h"

#include "stored.h"
#include "storednumeric.h"
#include "storedlabel.h"

struct SerialConfiguration:public Stored {
  StoredLabel device;//:"/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A1000pPj-if00-port0",
  StoredCount baud;//  baud:115200,
  StoredCount parity;//todo: enumerize 0:"none", 1:odd 2:even 3:mark 4:space
  StoredCount bits;//:8,
  StoredCount stop;//:1
  SerialConfiguration(Storable &node);
};

class SerialDevice {
  Text name;//retain for debug
public:
  Fildes fd;
  SerialDevice();
  bool connect(const SerialConfiguration &cfg);
  void close();

//  //for skewless updates:
//  struct DigitalOutputs {
//    unsigned DSR:1;
//    unsigned CTS:1; //don't fiddle this unless you know you have turned off hardware handshaking
//    //on most hardware the following are only inputs
//    unsigned DCD:1;
//    unsigned RI:1;
//    //more later
//  };
//  bool updateDigitalOutputs(const DigitalOutputs &bits);

  class Pin {
  public:
    enum Which {
      //these are usually outputs
      Dtr,Rts,
      //and these are usually inputs, but ioctl doesn't care
      Dsr,Cts,
      Dcd, Ri,
    };
private:
    Fildes fd;
    int which;
    int pattern;
    bool invert;
    bool lastSet;
  public:
    Pin(const Fildes &fd,Which one,bool invert);
    virtual ~Pin()=default;
    operator bool() noexcept;
    bool operator =(bool on)noexcept;
    virtual void toggle() noexcept{
      *this=1-lastSet;//opposite of last request, in case read doesn't work.
    }
  };

  /** @returns new access structure that makes a handshake line usable as a GPIO. */
  SerialDevice::Pin *makePin(SerialDevice::Pin::Which one,bool invert);
};

#include "abstractpin.h"
class SPIO: public AbstractPin {
  SerialDevice::Pin &raw;
public:
  SPIO(SerialDevice::Pin &raw);

public:// AbstractPin interface
  void operator =(bool value)  noexcept override;
  operator bool() noexcept override;
  void toggle() noexcept override;
};

#endif // SERIALDEVICE_H
