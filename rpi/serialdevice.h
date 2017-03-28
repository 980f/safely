#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include "fildes.h"

#include "stored.h"
#include "storednumeric.h"
#include "storedlabel.h"

struct SerialConfiguration:public Stored {
  StoredLabel device;//:"/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A1000pPj-if00-port0",
  StoredInt baud;//  baud:115200,
  StoredInt parity;//todo: enumerize 0:"none", 1:odd 2:even 3:mark 4:space
  StoredInt bits;//:8,
  StoredInt stop;//:1
  SerialConfiguration(Storable &node);
};

class SerialDevice {
  Text name;//retain for debug
public:
  Fildes fd;
  SerialDevice();
  bool connect(const SerialConfiguration &cfg);
  void close();
};

#endif // SERIALDEVICE_H
