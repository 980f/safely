#include "iosource.h"
//#include "glibmm/iochannel.h"
//#include "glibmm/refptr.h"
//#include "glibmm/main.h" //timeout and other small glib classes

IoSource::IoSource(int fd):
  fd(fd){
  //#nada
}

IoSource::~IoSource(){
  //created for breakpoint
}


int IoSource::recode(ssize_t rwreturn){
  if(rwreturn==~0){
    int errnum(errno);
    if(EINTR == errnum || EAGAIN == errnum || EWOULDBLOCK == errnum) {
      return 0;//still more perhaps.
    }
    else {
      return -errnum;
    }
  } else {
    return rwreturn;
  }
}

