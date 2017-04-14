#include "posixwrapper.h"
#include "cheaptricks.h"
#include <errno.h>
//#include <syslog.h>
#include "logger.h"
#include <stdarg.h> //for varargs logging
#include "string.h" //for strerror

bool PosixWrapper ::needsInit=true;

PosixWrapper::PosixWrapper(const char *prefix):dbg(prefix){
  if(flagged(needsInit)){
//    openlog("APP",LOG_CONS | LOG_PERROR,LOG_USER);
  }
}

void PosixWrapper::logmsg(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  dbg.varg(fmt, args);
  va_end(args);
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
    //hopefully strerror handles bogus input.
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
      logmsg("Failed: %s",errorText());//former code only worked when errno was the actual source of the error.
    }
//    else {
//      syslog(debug, "Cleared previous error");
//    }
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

//bool PosixWrapper::okValue(int &updatee, int valorminus1){
//  updatee=valorminus1;
//  if(valorminus1<0){
//    return failure(errno);
//  } else {
//    return true;
//  }
//}

//end of file
