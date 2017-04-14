#include "activityperformance.h"

ActivityPerformance::ActivityPerformance():
  pt(false)//don't be running before first use, confuses debug
{
  //#nada
}

bool ActivityPerformance::track(bool goodly){
  double elapsed=pt.elapsed();
  if(goodly){
    good.inspect(elapsed);
    return true;
  } else {
    bad.inspect(elapsed);
    return false;
  }
}
