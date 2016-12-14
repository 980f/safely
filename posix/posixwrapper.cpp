#include "posixwrapper.h"
#include "cheapTricks.h"
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>

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

bool PosixWrapper::failure(bool passthrough){
  errornumber = errno;
  if(errno != 0) {
    syslog(debug, "Failure: %m");
  }
  return passthrough;
}


bool PosixWrapper::failed(int zeroorminus1){
  if(zeroorminus1 == -1){
    if(changed(errornumber ,errno)){
      syslog(debug, "Failed: %m");
    }
    return true;

  } else {
    errornumber =0;
    return false;
  }
}

//end of file
