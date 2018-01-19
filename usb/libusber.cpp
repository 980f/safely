//(C) 2017 Andrew L. Heilveil
#include "libusber.h"
#include "minimath.h"

#include "microseconds.h"

#if 0

void UsbDevice::addWatch(int fd, short events){
  dbg("Add watch for fd=%d, events=%x",fd,events);
  //todo:g check chain for existing IOsource for that fd, if none create one.
  if(events & POLLIN) {
    dbg("Now watching file number %d for input",fd);
    // for glib this is where we will build an IOSource object and listen to it and call  our background()
  }
  if(events & POLLOUT) {
    dbg("Now watching file number %d for output",fd);
  }
} // UsbDevice::addWatch

void UsbDevice::removeWatch(int fd){
  dbg("Stop watching file number %d",fd);
  //check chain for existing IOsource for fd, but don't create one.
}

void UsbDevice::pollfd_added(int fd, short events, void *user_data){
  reinterpret_cast<UsbDevice *>(user_data)->addWatch(fd, events);
}

void UsbDevice::pollfd_removed(int fd, void *user_data){
  reinterpret_cast<UsbDevice *>(user_data)->removeWatch(fd);
}

#endif // if 0

static const char * errormessages[] {
  "Success",
  "Input/output error",
  "Invalid parameter",
  "Access denied (insufficient permissions)",
  "No such device (it may have been disconnected)",
  "Entity not found",
  "Resource busy",
  "Operation timed out",
  "Overflow",
  "Pipe error",
  "System call interrupted (perhaps due to signal)",
  "Insufficient memory"
  "Operation not supported or unimplemented on this platform",
//new ones go here

  "Other error"
};


LibUsber::LibUsber() : PosixWrapper("UsbLib"){
  //libusb error codes are negative, PosixWrapper will use the following for those.
  alttext = errormessages;
  numalts = countof(errormessages);
}

bool LibUsber::init(){
  if(!ctx) {
    return !failure(libusb_init(&ctx));
  } else {
    return true; //todo: is there a 'ok to use' member?
  }
}

void LibUsber::setDebugLevel(int lusbLevel){
  libusb_set_debug(ctx, lusbLevel); // set verbosity level
}

MicroSeconds LibUsber::doEvents(){
  MicroSeconds tv;
  libusb_handle_events_timeout_completed(ctx, &tv,&completed);

  int hasTimeout = libusb_get_next_timeout(ctx,&tv );
  if(hasTimeout<0) {
    failure(hasTimeout);
    return 0;
  } else if(hasTimeout==1) {
    return tv;
  } else {
    return 0;
  }
}

int LibUsber::buriedError() const {
//  if(ctx){ //too hidden at the moment.
//    return ctx->debug_fixed;
//  } else {
    return 0;
//  }
} // LibUsber::doEvents


#define RecordIds devid.id.vendor = idVendor; devid.id.product = idProduct

#include "onexit.h"
bool LibUsber::find(uint16_t idVendor,uint16_t idProduct,unsigned nth){
  RecordIds;
  libusb_device **devs = nullptr;

  OnExit release([devs] (){
    libusb_free_device_list(devs, 1);//libusb_open ++'s the ref count for the one we keep a ref to.
  });
  ssize_t devcnt = libusb_get_device_list(nullptr, &devs);

  if (devcnt < 0) {
    errornumber = int(devcnt);//#cast error code
    dbg("failed to get device list, %d",errornumber);//todo: translate error code for message.
    return false;
  }

  for (unsigned matched = 0,iDev = 0; iDev < unsigned(devcnt); ++iDev) {
    libusb_device_descriptor desc;
    libusb_device *device = devs[iDev];
    libusb_get_device_descriptor(device, &desc);//latest version never fails

    if ((desc.idVendor == idVendor) && (desc.idProduct == idProduct)) {
      if (nth ==0 || ++matched==nth) {
        libusb_device *found = device;
        if(failure(libusb_open(found, &devh))) {
          dbg("%s",errorText());
          devh = nullptr;//to be sure
          return false;//found but couldn't open
        } else {
          ++reconnects;
          return true;
        }
      }
    }
  }
  return false;//none found
} // LibUsber::find

bool LibUsber::open(uint16_t idVendor, uint16_t idProduct){
  RecordIds;
  devh = libusb_open_device_with_vid_pid(ctx,idVendor,idProduct);
  if(devh!=nullptr) {//#spread for debug, do not optimize.
    ++reconnects;
    plugWatcher(true);
    return true;
  } else {
    this->errornumber=buriedError();
    return false;
  }
}

bool LibUsber::claim(int desiredInterfacenumber){
  //we are trusting that there is only one configuration and that it has our desiredInterfacenumber
  if(failure(libusb_claim_interface(devh, desiredInterfacenumber))) {
    dbg("Failed to claim interface %d",desiredInterfacenumber);
    return false;
  } else {
    claimedInterface = desiredInterfacenumber;
    return true; // have interface
  }
}

bool LibUsber::submit(libusb_transfer *xfer){
  if(devh==nullptr){
    return false;//segv on power down.
  }
  if(xferInProgress) {
    dbg("attempting to overlap submissions, not yet allowed");//did you forget to ack()?
    return false;
  }
  if(xfer) {
    xfer->dev_handle = devh;
    xferInProgress = xfer;

    if(failure(libusb_submit_transfer(xfer))) {//#non-blocking call
      if(xfer->last_errno!=0 && xfer->last_errno!=errornumber) {
        failure(xfer->last_errno);//above 'hides' core errno. //22 invalid parameter
      }
      ack(xfer);
      return false;
    } else {
      //the xfer has callback info to notify the submitter of progress
      return true;//preferred exit
    }
  } else {
    //todo:1 cancel current xfer, presently not allowed
    return false;
  }
} // LibUsber::submit

bool LibUsber::ack(libusb_transfer *xfer){
  if(xferInProgress) {
    if(xfer==xferInProgress) {
      xferInProgress = nullptr;
      return true;
    } else {
      return false;
    }
  } else {
    return xfer==nullptr;
  }
}

void LibUsber::releaseHandle(){
  if(devh){
    ++disconnects;
  }
  libusb_close(take(devh));//fail fast on use after close
  plugWatcher(false);
}

int LibUsber::onPlugEvent(libusb_hotplug_event event){
  switch(event) {
  case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
    dbg("Hotplug arrival:[%04X:%04X]",devid.id.vendor,devid.id.product);
    if(devh){
      wtf("Hotplug while still plugged");
      releaseHandle();
    }
    open(devid.id.vendor,devid.id.product);
    break;
  case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
    dbg("Hotplug departure: [%04X:%04X]",devid.id.vendor,devid.id.product);
    if(xferInProgress){
      libusb_cancel_transfer(xferInProgress);
      //which should call our callback which should clear xferInProgress
      //but in case that fails
      xferInProgress=nullptr;//forget all state related to device.
    }
    releaseHandle();
    plugWatcher(false);
    break;
  }
  return 0;//keep ourselves around.
} // LibUsber::onPlugEvent

static int plugcallback(struct libusb_context */*ctx*/, struct libusb_device */*dev*/,libusb_hotplug_event event, void *user_data){
  LibUsber &user(*reinterpret_cast<LibUsber*>(user_data));
  //we only register for our context and our device so we can ignore those operands
  return user.onPlugEvent(event);
}

bool LibUsber::watchplug(uint16_t idVendor, uint16_t idProduct,Hook<bool> hooker){
  RecordIds;
  plugWatcher=hooker;
  if(failure(libusb_hotplug_register_callback(ctx,libusb_hotplug_event( LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),LIBUSB_HOTPLUG_ENUMERATE,devid.id.vendor, devid.id.product,LIBUSB_HOTPLUG_MATCH_ANY,  plugcallback, this, &hotplugger))) {
    dbg("Failed to register hotplug watcher");
    return false;
  } else {
    dbg("hotplug handle: %08X",hotplugger);
    return true;
  }

} // LibUsber::ack

bool LibUsber::close(){
  if (devh) {
    if(failure(libusb_release_interface(devh, claimedInterface))) {
      dbg("libusb_release_interface error %s\n", errorText());
      claimedInterface = ~0;//fail hard
    }
    releaseHandle();
    return true;
  } else {
    return false;
  }
}

LibUsber::~LibUsber(){
  close();
  libusb_exit(take(ctx));//take():fail faster on use after free.
}
