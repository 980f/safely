#ifndef TIMERFD_H
#define TIMERFD_H
/** timer service via file descriptor */
#include "posixwrapper.h"
#include "fildes.h"
class TimerFD:public PosixWrapper {
  Fildes fd;
public:
  TimerFD();
  /** set period and initial delay to same value */
  double setPeriod(double seconds);
  /** acknowledge a timer event, so that it can signal again */
  void ack();

  int asInt()const{
    return fd.asInt();
  }
};

#endif // TIMERFD_H
