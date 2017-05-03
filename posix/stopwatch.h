#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "nanoseconds.h"

/** an interval timer */
class StopWatch {
private:
  /** first time of interest in this run of the application.
   * At the moment that is when the first StopWatch is created, even if it is later destroyed.
   * This is used to reduce the 'number of seconds' used in some calculations to make sure nanoseconds don't get discarded when added to seconds.
*/
  static __time_t epoch;
private:
  void readit(timespec &ts);
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
  double roll(double *absolutely=nullptr);
  /** use start and stop for non-periodic purposes*/
  void start();
  void stop();
  /** convenient for passing around 'timeout pending' state */
  bool isRunning() const;
  /** updates 'stop' if running then @returns time between start and stop as seconds. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  double elapsed(double *absolutely=nullptr);

  /** make last 'elapsed' be a start, retroactively (without reading the system clock again.*/
  void rollit();
  /** @return seconds of absolute time of stop, or now if running*/
  double absolute();

  /** @returns the number of cycles of frequency @param atHz that have @see elapsed() */
  unsigned cycles(double atHz, bool andRoll=true);
};

#endif // STOPWATCH_H
