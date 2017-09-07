//(C) 2017 Andrew L. Heilveil
#include "usbid.h"


UsbId::UsbId(Storable &node, unsigned dvid, unsigned dpid,unsigned serial):Stored(node),
  ConnectChild(vid,dvid),
  ConnectChild(pid,dpid),
  ConnectChild(serialNumber,serial) {
  //#nada
}
