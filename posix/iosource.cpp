#include "iosource.h"

IoSource::IoSource(const char * tracename, int fd):
  Fildes(tracename){
  if (fd>=0) {
    preopened(fd,false);//if preopened we don't own it. But creator of this IoSource can pass ownership to it later.
  }
}

IoSource::~IoSource(){
  //make sure Fildes destructor is called.
}


int IoSource::recode(ssize_t rwreturn) {
  if (rwreturn == ~0) {
    int errnum(errno);
    if (EINTR == errnum || EAGAIN == errnum || EWOULDBLOCK == errnum) {
      return 0; // still more perhaps.
    }
    return -errnum;
  }
  return rwreturn;
}
bool IoConnections::writeInterest(IoSource::Slot action) {
  //todo: replace this concept with asynchio?
  return false;
}

