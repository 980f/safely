#ifndef PERFTIMER_H
#define PERFTIMER_H

#include "textpointer.h"
#include "stopwatch.h"

/** A stopwatch with start and stop tied to construction and destruction.
* Those events log messages keyed with text provided to the constructor.
* starts timer when constructed, reports elapsed time when destructed. */
struct PerfTimer {
  StopWatch swatch;
  Text note;
  bool enabled;
public:
  /** global/master enable */
  static bool all;
  static bool showStarts;
public:
  /** NB: the @param note content is copied.*/
  PerfTimer(TextKey note, bool enabled = true);
/** stops timer and logs message */
  ~PerfTimer();
  /** handy for constructor timing, when sub-constructors are to be timed. */
  void elapsed();
};

//a macro for timing a function, just add this one line where timing should start.
#define TIMED_FUNCTION(whichenable) PerfTimer perf(__PRETTY_FUNCTION__,whichenable)

#endif // PERFTIMER_H
