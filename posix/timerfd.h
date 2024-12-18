#pragma once // "(C) Andrew L. Heilveil, 2017"

/** timer service via file descriptor, so that we can use epoll to get events rather than deal with signals.
 * Note: a zero delay will NOT trigger immediately, use at list 1 us for that.
 *
 * man timerfd_create for documentation on the mechanism.
 *
 * todo: use TFD_TIMER_CANCEL_ON_SET  to make it a "one shot"
 */

#include "fildes.h"
#include <nanoseconds.h>

class TimerFD : public Fildes {
  /** whether to try to stay in phase, vs minimum interval. */
  bool phaseLock;
  NanoSeconds period;

public:
  /** this value is the minimum to guarantee that the timerFD will indicate a timeout in the next or subsequent poll.*/
 static constexpr NanoSeconds minimum{0.000'000'001};


  TimerFD(const char *traceName, bool phaseLock = false); //false: legacy default
  /** set period and initial delay to same value
   * @returns prior period value */
  NanoSeconds setPeriod(NanoSeconds seconds);

  /** acknowledge a timer event, so that it can signal again.
 @returns whether there was an event active, by getting a response to the 'read overflow count' operation */
  bool ack();

  NanoSeconds getPeriod() const noexcept;

  NanoSeconds getExpiration() const noexcept;

  /** @returns number of intervals that will elapse for a given frequency's cycle time */
  unsigned chunks(double hz);

  /** stop the timer, will not get reports */
  itimerspec pause();

  /** resumes time, but does not compensate for time spent between pause and now. */
  void resume(const itimerspec &was);

  void cancel() {
    pause();//cheap to write, costly to run, but not too costly.
  }

  /** automatic resume on object destruction. */
  struct Pauser {
    TimerFD &timerFD;
    itimerspec was;  //the only access we have is to this type, not the pieces therein.
    bool paused = false;

    void pause() {
      was = timerFD.pause();
    }

    void resume() {
      timerFD.resume(was);
    }

    Pauser(TimerFD &timerFD, bool pauseNow = true) : timerFD(timerFD) { // LINT: was's validity is tracked by 'paused' and so we can fail to init it without concern.
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
