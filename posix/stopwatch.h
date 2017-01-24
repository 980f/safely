#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>

/** an interval timer */
class StopWatch {
private:
  /** first time of interest in this run of the application.
   * At the moment that is when the first StopWatch is created, even if it is later destroyed.
   * This is used to reduce the 'number of seconds' used in some calculations to make sure nanoseconds don't get discarded when added to seconds.
*/
  static __time_t epoch;
public:
  /** timestamp reported as seconds since roughly the start of the application */
  double asSeconds(const timespec &ts);
private:
  void readit(timespec &ts);
  const int CLOCK_something;
protected:
  timespec started;
  timespec stopped;
  bool running;
public:
  /** @param beRunning is whether to start timer upon construction.
      @param realElseProcess is whether to track realtime or thread-active-time */
  StopWatch(bool beRunning=true,bool realElseProcess=false);//defaults are best choice for function timing.

  /** @returns elasped time and restarts interval. Use this for cyclic sampling. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  double roll(double *absolutely=0);
  /** use start and stop for non-periodic purposes*/
  void start();
  void stop();
  /** convenient for passing around 'timeout pending' state */
  bool isRunning() const;
  /** updates 'stop' if running and @returns time between start and stop as seconds. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  double elapsed(double *absolutely=0);
  /** @return seconds of absolute time of stop, or now if running*/
  double absolute();
};

#endif // STOPWATCH_H
