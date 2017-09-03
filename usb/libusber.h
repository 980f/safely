#ifndef LIBUSBER_H
#define LIBUSBER_H "(C) 2017 Andrew L. Heilveil"

#include <libusb.h>
#include "posixwrapper.h" //error returns are similar enough, just take care to call the correct errorText method.
#include "cstr.h"

class LibUsber : public PosixWrapper {
  libusb_context * ctx=nullptr;
public:
  /** nothing happens on construction */
  LibUsber();
  /** unclaim , close things */
  virtual ~LibUsber();

  bool init();
  void setDebugLevel(int lusbLevel= LIBUSB_LOG_LEVEL_WARNING);
  int completed=0;
  /** must be called periodically, or when related fd changes.
   * @returns seconds til next calback required. if Nan or < 0 then ignore value.*/
  double doEvents();
  /** last configuration explicitly set by methods of this class */
  int configuration=0;

  libusb_device_handle *devh=nullptr;
  /** record interface number if successfully claimed*/
  int claimedInterface=~0;

public:
  /** find by vid and pid, if more than one choose nth instance, open it. sets device handle and @returns success.
  */
  bool find(uint16_t idVendor, uint16_t idProduct, unsigned nth=1);

  /** find by vid and pid, only works if there is only one . sets device handle and @returns success
@deprecated untested */
  bool open(uint16_t idVendor, uint16_t idProduct);


  /** once you @see find the device you must claim an interface @returns success
@deprecated untested */
  bool claim(int desiredInterfacenumber=0);
/** initiate a xfer @deprecated untested */
  bool submit(libusb_transfer *xfer);
  /** call when you are truly finished with xfer */
  bool ack(libusb_transfer *xfer);
private:
  libusb_transfer *xferInProgress=nullptr;
};


#endif // LIBUSB_H
