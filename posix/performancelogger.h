#ifndef PERFORMANCELOGGER_H
#define PERFORMANCELOGGER_H

#include "fildes.h"
#include "stopwatch.h"
#include "textkey.h"

/** instantiation starts timer, destructor emits logging message.
@see perftimer and performancetimer for variants on output method */
class PerformanceLogger{
  StopWatch perf;
  TextKey format;
  Fildes &replyto;

public:
  /** @deprecated untested */
  PerformanceLogger(TextKey format,Fildes &replyto);
  ~PerformanceLogger();
  void onDone();
};

#endif // PERFORMANCELOGGER_H
