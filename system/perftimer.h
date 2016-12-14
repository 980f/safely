#ifndef PERFTIMER_H
#define PERFTIMER_H

#include "textpointer.h"
#include "stopwatch.h"

/** starts timer when constructed, reports elapsed time when destructed. */
struct PerfTimer {
  StopWatch swatch;
  TextPointer note;
  bool enabled;
public:
  static bool all;//global enable
public:
  /** NB: the @param note pointer is retained, make sure the buffer isn't deallocated after construction of this timer.*/
  PerfTimer(TextKey note, bool enabled = true);

  ~PerfTimer();
  /** handy for constructor timing, when sub-constructors are to be timed. */
  void elapsed();
};


#define TIMED_FUNCTION(whichenable) PerfTimer perf(__PRETTY_FUNCTION__,whichenable)


#endif // PERFTIMER_H
