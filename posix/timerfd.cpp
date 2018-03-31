//"(C) Andrew L. Heilveil, 2017"
#include "timerfd.h"

#include "sys/timerfd.h"
#include "nanoseconds.h"

#include "minimath.h"

TimerFD::TimerFD(bool phaseLock):PosixWrapper ("TimerFD"),
  fd("TimerFD"),
  phaseLock(phaseLock){
  int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
  fd.preopened(tfd,true);
  period=getPeriod();
}

double TimerFD::setPeriod(double seconds){
  if(fd.isOk()){
    itimerspec u;
    parseTime(u.it_interval,seconds);
    parseTime(u.it_value,seconds);//set initial delay to same as period

    itimerspec old;
    timerfd_settime(fd.asInt(),phaseLock?TFD_TIMER_ABSTIME:0,&u,&old);//0: not TFD_TIMER_ABSTIME, todo: add option for absolute time.
    return period=from(old.it_interval);
  }
  return Nan;
}

bool TimerFD::ack(){
  u64 expirations=~0UL;//type here is chosen by timer fd stuff, not us.
  ByteScanner discard(reinterpret_cast<u8*>(&expirations),sizeof(expirations));
  fd.read(discard);//todo:1 why does fd.read return 0 even when it read 8 bytes?
  if(sizeof(expirations)==fd.lastRead){
    return true;
  } else {
    //then we shouldn't have been called.
    return false;
  }
}

double TimerFD::getPeriod() const noexcept {
  itimerspec old;
  timerfd_gettime(fd.asInt(),&old);
  return from(old.it_interval);
}

unsigned TimerFD::chunks(double hz){
  return ::chunks(ratio(1.0,hz), period);
}
