#include "performancetimer.h"


PerformanceTimer::PerformanceTimer(Cstr prefix, Logger *logger):
  prefix(prefix),
  logger(logger)
{
  timer.start();
}

PerformanceTimer::~PerformanceTimer(){
  if(logger==nullptr){
    logger=&dbg;
  }
  (*logger)("%s took %g seconds",prefix.c_str(),timer.elapsed());
}
