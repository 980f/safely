#include "posixwrapper.h"
#include "cheaptricks.h"  //changed()
#include <errno.h>
#include "string.h" //for strerror


PosixWrapper::PosixWrapper(const char *prefix):dbg(prefix){
  //#nada
}

const char *PosixWrapper::errorText() const {
  if(errornumber<0){
    if(alttext&&numalts){
      if(unsigned(-errornumber)<numalts){
        return alttext[-errornumber];
      } else {
        return alttext[numalts-1];
      }
    }
  }
  return strerror(errornumber);
}

bool PosixWrapper::setFailed(bool passthrough){
  if(passthrough){
    failure(errno);
  }
  return passthrough;
}

bool PosixWrapper::failure(int errcode){
  if(changed(errornumber,errcode)) {//only log message if different than previous, prevents spam at the loss of occasional meaningful duplicates.
    // If you think you might repeat an error then clear errornumber before such a call.
    if(errcode!=0){
      dbg("Failed: %s",errorText());//former code only worked when errno was the actual source of the error.
    }
  }
  return errcode!=0;
}

bool PosixWrapper::failed(int zeroorminus1){
  if(zeroorminus1 == -1) {
    return failure(errno);
  } else {
    errornumber = 0;
    return false;
  }
}

bool PosixWrapper::isWaiting(){
  return errornumber==EAGAIN || errornumber== EWOULDBLOCK;
}


