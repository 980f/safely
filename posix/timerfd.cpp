//"(C) Andrew L. Heilveil, 2017"
#include "timerfd.h"
#include "sys/timerfd.h"
#include "nanoseconds.h"
#include "minimath.h" //chunks

TimerFD::TimerFD(const char *traceName, bool phaseLock): Fildes(traceName ? traceName : "TimerFD"),
  phaseLock(phaseLock) {
  preopened(timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK), true);
  period = getPeriod();
}

NanoSeconds TimerFD::setPeriod(NanoSeconds seconds) {
  period = seconds;
  if (isOk()) {
    itimerspec u;
    u.it_interval = seconds;
    u.it_value = seconds; //set initial delay to same as period

    itimerspec old;
    timerfd_settime(fd, phaseLock ? TFD_TIMER_ABSTIME : 0, &u, &old); //0: no flags
    seconds = old.it_interval;
    return seconds;
  }
  return NanoSeconds();
}

bool TimerFD::ack() {
  auto expirations = read(uint64_t(~0)); //#type here is chosen by timer fd stuff, not us.

  if (expirations) { //# expanded for breakpoint.
    return true;
  } else { //then we shouldn't have been called.
    return false;
  }
}

NanoSeconds TimerFD::getPeriod() const noexcept {
  itimerspec old;
  timerfd_gettime(fd, &old);
  return NanoSeconds(old.it_interval);
}

NanoSeconds TimerFD::getExpiration() const noexcept {
  itimerspec old;
  timerfd_gettime(fd, &old);
  return NanoSeconds(old.it_value);
}

itimerspec TimerFD::pause() {
  itimerspec old;
  itimerspec u;
  /* from man page: "Setting both fields of new_value.it_value to zero disarms the timer."*/
  u.it_value.tv_sec = 0;
  u.it_value.tv_nsec = 0;
  timerfd_settime(fd, 0, &u, &old);
  return old;
}

void TimerFD::resume(const itimerspec &was) {
  //this version does not deal with time elapsed while paused. For that we need to play abstime vs relative and that is quite tedious.
  timerfd_settime(fd, 0, &was, nullptr);
}


unsigned TimerFD::chunks(double hz) {
  return ::chunks(ratio(1.0, hz), period);
}
