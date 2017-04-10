#include "timerfd.h"

#include "sys/timerfd.h"
#include "nanoseconds.h"

TimerFD::TimerFD(){
  int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
  fd.preopened(tfd,true);
//  itimerspec u;
//  parseTime(u.it_interval,0);
//  parseTime(u.it_value,0);

//  timerfd_settime(fd.asInt(),0,&u,nullptr);

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
  u8 value[8];
  ByteScanner discard(value,sizeof(value));
  if(8==fd.read(discard)){
    return true;
  } else {
    //then we shouldn't have been called.
    return false;
  }
}
