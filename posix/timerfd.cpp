#include "timerfd.h"

#include "sys/timerfd.h"
#include "nanoseconds.h"

TimerFD::TimerFD(){
  int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
  fd.preopened(tfd,true);
}

double TimerFD::setPeriod(double seconds){
  if(fd.isOk()){
    itimerspec u;
    parseTime(u.it_interval,seconds);

    itimerspec old;
    timerfd_settime(fd.asInt(),0,&u,&old);
    return from(old.it_interval);
  }
  return Nan;
}

void TimerFD::ack(){
  u8 value[8];
  ByteScanner discard(value,sizeof(value));
  if(8!=fd.read(discard)){
    //then we shouldn't have been called.
  }
}
