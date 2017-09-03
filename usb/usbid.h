#ifndef USBID_H
#define USBID_H "(C) 2017 Andrew L. Heilveil"

#include "storednumeric.h"

struct UsbId: public Stored {
  StoredNumeric<uint16_t> vid;
  StoredNumeric<uint16_t> pid;
  //will pick this up from device itself and report on mistmatch,
  StoredNumeric<uint16_t> serialNumber;//since device order isn't predictable we may have to pick by serial

  UsbId(Storable &node, unsigned dvid=0, unsigned dpid=0, unsigned serial=0);
};

#endif // USBID_H
