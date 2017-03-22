#include "posixwrapper.h"
#include "cheaptricks.h"
#include <errno.h>
#include <syslog.h>
#include <stdarg.h> //for varargs logging
#include "string.h" //for strerror


PosixWrapper::PosixWrapper(){
  errornumber = 0;
  debug = 0;
}

void PosixWrapper::logmsg(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  vsyslog(debug, fmt, args);
  va_end(args);
}

const char *PosixWrapper::errorText() const {
  return strerror(errornumber);
}

bool PosixWrapper::setFailed(bool passthrough){
  if(passthrough){
    failure(errno);
    if(errornumber != 0) {
      syslog(debug, "Failure: %m");
    }
  }
  return passthrough;
}

bool PosixWrapper::failure(int errcode){
  if(changed(errornumber,errcode)) {//only log message if different than previous, prevents spam at the loss of occasional meaningful duplicates.
    // If you think you might repeat an error then clear errornumber before such a call.
    syslog(debug, "Failed: %m");
  }
  return errcode!=0;
}

bool PosixWrapper::failed(int zeroorminus1){
  if(zeroorminus1 == -1) {
    if(changed(errornumber,errno)) {//only log message if different than previous, prevents spam at the loss of occasional meaningful duplicates.
      // If you think you might repeat an error then clear errornumber before such a call.
      syslog(debug, "Failed: %m");
    }
    return true;
  } else {
    errornumber = 0;
    return false;
  }
}

bool PosixWrapper::okValue(int &updatee, int valorminus1){
  updatee=valorminus1;
  if(valorminus1<0){
    return failure(errno);
  } else {
    return true;
  }
} // PosixWrapper::failed

//end of file
