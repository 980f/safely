#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "nanoseconds.h"

/** an interval timer.
 todo:1 elapsed additional return as NanoSeconds, not just double. */
class StopWatch {

private:
  NanoSeconds epoch;//saves time of construction

  bool readit(timespec &ts);
  const int CLOCK_something;
protected:
  NanoSeconds started;
  NanoSeconds stopped;

  bool running;
public:
  /** @param beRunning is whether to start timer upon construction.
      @param realElseProcess is whether to track realtime or thread-active-time.
NB: thread time is only sane if the StopWatch stays associated with the same physical CPU in a SMP system.
Most of the time 'real' makes more sense, but when debugging 'process' time is more like what will be encountered when not debugging.
 */
  StopWatch(bool beRunning=true,bool realElseProcess=false);//defaults are for performance timing.

  /** @returns elasped time and restarts interval. Use this for cyclic sampling. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  NanoSeconds roll(double *absolutely=nullptr);
  /** use start and stop for non-periodic purposes*/
  void start();
  /** stops acquiring (if not already stopped) and @returns REFERENCE to stopped tracker. You probably want elapsed() or rollit(), this guy is only needed for some time critical timing situations */
  void stop();
  /** convenient for passing around 'timeout pending' state */
  bool isRunning() const;
  /** updates 'stop' if running then @returns time between start and stop as seconds. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  NanoSeconds elapsed(double *absolutely=nullptr);

  /** make last 'elapsed' be a start, retroactively (without reading the system clock again.*/
  void rollit();
  /** @return seconds of absolute time of stop, or now if running*/
  NanoSeconds absolute();

  /** @returns the number of cycles of frequency @param atHz that have @see elapsed() */
  unsigned cycles(double atHz, bool andRoll=true);
  /** @returns how many intervals have passed, and if andRoll sets start modulo interval */
  unsigned periods(NanoSeconds interval, bool andRoll=true);
  /** @return last clock value sampled, either as absolute (time since program start) or since stopwatch.start()*/
  NanoSeconds lastSnap(bool absolute=false) const;
};

#endif // STOPWATCH_H
