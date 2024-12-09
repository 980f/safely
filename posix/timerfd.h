#pragma once // "(C) Andrew L. Heilveil, 2017"

/** timer service via file descriptor, so that we can use epoll to get events rather than deal with signals.
 *
 * man timerfd_create for documentation on the mechanism.
 */
#include "posixwrapper.h"
#include "fildes.h"

class TimerFD : public PosixWrapper {
public: //handy for debug
  Fildes fd;

private:
  /** whether to try to stay in phase, vs minimum interval. */
  bool phaseLock;
  double period;

public:
  TimerFD(const char *traceName, bool phaseLock = false); //false: legacy default
  /** set period and initial delay to same value */
  double setPeriod(double seconds);

  /** acknowledge a timer event, so that it can signal again.
 @returns whether there was an event active, by getting a response to the 'read overflow count' operation */
  bool ack();

  int asInt() const {
    return fd.asInt();
  }

  double getPeriod() const noexcept;

  void operator=(bool enable);

  /** @returns number of intervals that will elapse for a given frequency's cycle time */
  unsigned chunks(double hz);
};
