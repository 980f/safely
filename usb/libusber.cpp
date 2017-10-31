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
}

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

#endif

static const char * errormessages[]{
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


LibUsber::LibUsber():PosixWrapper ("UsbLib"){
  //libusb error codes are negative, PosixWrapper will use the following for those.
  alttext=errormessages;
  numalts=countof(errormessages);
}


bool LibUsber::init(){
  if(!ctx){
    return !failure(libusb_init(&ctx));
  } else {
    return true; //todo: is there a 'ok to use' member?
  }
}

void LibUsber::setDebugLevel(int lusbLevel){
  libusb_set_debug(ctx, lusbLevel); // set verbosity level
}

double LibUsber::doEvents(){
  MicroSeconds tv;
  libusb_handle_events_timeout_completed(ctx, &tv,&completed);

  int hasTimeout= libusb_get_next_timeout(ctx,&tv );
  if(hasTimeout<0){
    failure(hasTimeout);
    return -Infinity;
  }  else if(hasTimeout==1){
    return tv;
  } else {
    return Nan;
  }
}


#include "onexit.h"
bool LibUsber::find(uint16_t idVendor ,uint16_t idProduct,unsigned nth){
  libusb_device **devs=nullptr;

  OnExit release([devs](){
    libusb_free_device_list(devs, 1);//libusb_open ++'s the ref count for the one we keep a ref to.
  });
  ssize_t devcnt=libusb_get_device_list(nullptr, &devs);

  if (devcnt < 0) {
    errornumber=devcnt;
    dbg("failed to get device list, %s",errornumber);
    return false;
  }

  for (unsigned matched=0,iDev = 0; iDev < unsigned(devcnt); ++iDev) {
    libusb_device_descriptor desc;
    libusb_device *device = devs[iDev];
    libusb_get_device_descriptor(device, &desc);//latest version never fails

    if ((desc.idVendor == idVendor) && (desc.idProduct == idProduct)) {
      if (nth ==0 || ++matched==nth) {
        libusb_device *found = device;
        if(failure(libusb_open(found, &devh))){
          dbg("%s",errorText());
          devh= nullptr;//to be sure
          return false;//found but couldn't open
        } else {
          return true;
        }
      }
    }
  }
  return false;//none found
}

bool LibUsber::open(uint16_t idVendor, uint16_t idProduct){
  devh=libusb_open_device_with_vid_pid(ctx,idVendor,idProduct);
  return devh!=nullptr;
}

bool LibUsber::claim(int desiredInterfacenumber){
  //we are trusting that there is only one configuration and that it has our desiredInterfacenumber
  if(failure(libusb_claim_interface(devh, desiredInterfacenumber))){
    dbg("Failed to claim interface %d",desiredInterfacenumber);
    return false;
  } else {
    claimedInterface=desiredInterfacenumber;
    return true; // have interface
  }
}

bool LibUsber::submit(libusb_transfer *xfer){
  if(xferInProgress){
    dbg("attempting to overlap submissions, not yet allowed");//did you forget to ack()?
    return false;
  }
  if(xfer){
    xfer->dev_handle=devh;
    //somewhere we need a timeout set:
    //  if ((data_out[2] == PID1_REQ_SCOPE_MISC_TO) && data_out[3] == PID2_SEND_DIAGNOSTIC_DATA_TO) {
    //    timeout = DP5_DIAGDATA_TIMEOUT;
    //  } else {
    //    timeout = DP5_USB_TIMEOUT;
    //  }
    //xfer->timeout=1011;//wag, will do stats
    xferInProgress=xfer;
    if(failure(libusb_submit_transfer(xfer))){
      failure(xfer->last_errno);//above 'hides' core errno. //22 invalid parameter
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
}

bool LibUsber::ack(libusb_transfer *xfer){
  if(xferInProgress){
    if(xfer==xferInProgress){
      xferInProgress=nullptr;
      return true;
    } else {
      return false;
    }
  } else {
    return xfer==nullptr;
  }
}

LibUsber::~LibUsber(){
  if (devh) {
    if(failure(libusb_release_interface(devh, claimedInterface))){
      dbg("libusb_release_interface error %s\n", errorText());
      claimedInterface=~0;//fail hard
    }
    libusb_close(take(devh));//fail fast on use after free
  }
  libusb_exit(take(ctx));//fail faster on use after free.
}

