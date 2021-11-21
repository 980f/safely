#ifndef LIBUSBER_H
#define LIBUSBER_H "(C) 2017 Andrew L. Heilveil"

#include <hook.h>
#include <libusb.h>
#include <microseconds.h>
#include "posixwrapper.h" //error returns are similar enough, just take care to call the correct errorText method.
#include "cstr.h"

/** use libusb without having to deal with allocation details. */
class LibUsber : public PosixWrapper {
  libusb_context * ctx=nullptr;
public:
  /** nothing happens on construction */
  LibUsber();
  /** unclaim , close things */
  virtual ~LibUsber();

  bool init();
  void setDebugLevel(int lusbLevel= LIBUSB_LOG_LEVEL_WARNING);

  /** an obscure item used by event poller, public solely for debug */
  int completed=0;
  /** must be called periodically, or when related fd changes.
   * @returns seconds til next calback required. if Nan or < 0 then ignore value.*/
  MicroSeconds doEvents();

  /** last configuration explicitly set by methods of this class */
  int configuration=0;

  libusb_device_handle *devh=nullptr;

  /** routines that return things have a tendency to hide the errors they encountered. alh has added code to poke that value where it can be found */
  int buriedError() const;

  /** device id, recorded when a handle is acquired, but kept around after close */
  union DeviceId {
    unsigned xid;
    struct pair {
      u16 vendor;
      u16 product;
    } id;
    DeviceId(){
      xid=BadIndex;
    }
  } devid;

  /** record interface number if successfully claimed*/
  int claimedInterface=~0;

  /** connection counters, hopefully never excede one or two, no guard exists against rollover */
  unsigned reconnects=0;
  unsigned disconnects=0;
public: //three ways to hook up with a device, depending upon your knowledge of it.
  /** find by vid and pid, if more than one choose nth instance, open it. sets device handle and @returns success. */
  bool find(uint16_t idVendor, uint16_t idProduct, unsigned nth=1);

  /** find by vid and pid, only works if there is only one . sets device handle and @returns success */
  bool open(uint16_t idVendor, uint16_t idProduct);

  /** open via hotplug mechanism, and automatically reopen it as events occur. */
  bool watchplug(uint16_t idVendor, uint16_t idProduct,Hook<bool>hooker);

  /** once you @see find the device you must claim an interface @returns success */
  bool claim(int desiredInterfacenumber=0);
  /** initiate a xfer */

  bool submit(libusb_transfer *xfer);
  /** call when you are truly finished with xfer */
  bool ack(libusb_transfer *xfer);
private:
  void releaseHandle();

  libusb_transfer *xferInProgress=nullptr;
  //lib usb callback handle, will autorelease when we ditch the context.
  libusb_hotplug_callback_handle hotplugger=0;
  //who to inform on plug events.
  Hook<bool> plugWatcher;

public://for thunking
  int onPlugEvent(libusb_hotplug_event event);
  /** @returns whether it actually did anything */
  bool close();
};

#endif // LIBUSB_H
