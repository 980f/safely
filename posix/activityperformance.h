#ifndef ACTIVITYPERFORMANCE_H
#define ACTIVITYPERFORMANCE_H

#include "stopwatch.h"
#include "extremer.h"

/** track max activity execution, separately for successes and failures */
struct ActivityPerformance {
  StopWatch pt;
  MaxDouble good;
  MaxDouble bad;
public:
  ActivityPerformance();
  void start(){
    pt.start();
  }

  /** record a time */
  bool track(bool goodly);
};

#endif // ACTIVITYPERFORMANCE_H
