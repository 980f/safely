#include "timerfd.h"

#include "sys/timerfd.h"
#include "nanoseconds.h"

TimerFD::TimerFD():PosixWrapper ("TimerFD"),fd("TimerFD"){
  int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
  fd.preopened(tfd,true);
}

double TimerFD::setPeriod(double seconds){
  if(fd.isOk()){
    itimerspec u;
    parseTime(u.it_interval,seconds);
    parseTime(u.it_value,seconds);//set initial delay to same as period

    itimerspec old;
    timerfd_settime(fd.asInt(),0,&u,&old);
    return from(old.it_interval);
  }
  return Nan;
}

bool TimerFD::ack(){
  u64 expirations=~0;
  ByteScanner discard(reinterpret_cast<u8*>(&expirations),sizeof(u64));
  fd.read(discard);
  if(8==fd.lastRead){//todo:1 why does fd.read return 0 even when it read 8 bytes?
    return true;
  } else {
    //then we shouldn't have been called.
    return false;
  }
}
