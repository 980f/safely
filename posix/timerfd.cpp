#include "timerfd.h"

#include "sys/timerfd.h"


TimerFD::TimerFD(){
  int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
  fd.preopened(tfd,true);
}

void TimerFD::setPeriod(double seconds){
  if(fd.isOk()){
    itimerspec u;
    u.it_interval.tv_sec=seconds;
    u.it_interval.tv_nsec=0;//seconds;

    itimerspec old;
    timerfd_settime(fd.asInt(),0,&u,&old);
  }
}

void TimerFD::ack(){
  u8 value[8];
  ByteScanner discard(value,sizeof(value));
  if(8!=fd.read(discard)){
    //then we shouldn't have been called.
  }
}
