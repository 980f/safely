#ifndef TIMERFD_H
#define TIMERFD_H
/** timer service via file descriptor */
#include "posixwrapper.h"
#include "fildes.h"
class TimerFD:public PosixWrapper {
public://handy for debug
  Fildes fd;
public:
  TimerFD();
  /** set period and initial delay to same value */
  double setPeriod(double seconds);
  /** acknowledge a timer event, so that it can signal again.
 @returns whether there was an event active, by getting a response to the 'read overflow count' operation */
  bool ack();

  int asInt()const{
    return fd.asInt();
  }

};

#endif // TIMERFD_H
