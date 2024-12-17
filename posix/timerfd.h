#pragma once // "(C) Andrew L. Heilveil, 2017"

/** timer service via file descriptor, so that we can use epoll to get events rather than deal with signals.
 *
 * man timerfd_create for documentation on the mechanism.
 *
 * todo: use TFD_TIMER_CANCEL_ON_SET as "one shot"
 */

#include "fildes.h"
#include <nanoseconds.h>

class TimerFD : public Fildes {
  /** whether to try to stay in phase, vs minimum interval. */
  bool phaseLock;
  NanoSeconds period;

public:
  TimerFD(const char *traceName, bool phaseLock = false); //false: legacy default
  /** set period and initial delay to same value
   * @returns prior period value */
  NanoSeconds setPeriod(NanoSeconds seconds);

  /** acknowledge a timer event, so that it can signal again.
 @returns whether there was an event active, by getting a response to the 'read overflow count' operation */
  bool ack();

  NanoSeconds getPeriod() const noexcept;

  NanoSeconds getExpiration() const noexcept;

  /** @deprecated  incomplete, at present it loses the desired periodicity and time remaining. one has to setPeriod to get it to run again at a known value. Need a pauser object rather than this syntax.*/
  void operator=(bool enable);

  /** @returns number of intervals that will elapse for a given frequency's cycle time */
  unsigned chunks(double hz);

  itimerspec && pause();

  void resume(const itimerspec & was);

  /** automatic resume on object destruction. */
  struct Pauser {
    TimerFD &timerFD;
    itimerspec was;
    bool paused=false;

    void pause() {
      was= timerFD.pause();
    }

    void resume() {
      timerFD.resume(was);
    }

    Pauser(TimerFD &timerFD,bool pauseNow=true) : timerFD(timerFD) {
      if (pauseNow) {
        pause();
      }
    }

    ~Pauser() {
      if (paused) {
        resume();
      }
    }
  };
};
