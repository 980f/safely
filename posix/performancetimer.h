#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

#include "logger.h"
#include "stopwatch.h"
#include "textpointer.h"
/** on creation stars timer, on destruction reports elapsed time */
class PerformanceTimer{
public:
  Text prefix;
  StopWatch timer;
  Logger *logger;
  PerformanceTimer(Cstr prefix,Logger *logger);

  ~PerformanceTimer();
};

#endif // PERFORMANCETIMER_H
