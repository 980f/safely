#include "worstcase.h"

#include "logger.h"

SafeLogger(worstCase,true);//default on as it isn't very noisy once things settled down.

void WorstCase::show(){
  if(latency.inspect(watcher.elapsed())) {
    worstCase(format.c_str(),1000.0*latency.max());//ms for legibility
  }
}

WorstCase::WorstCase(TextKey fmt, bool realElseProcess):
  watcher(false,realElseProcess),
  format(fmt){
  //#done
}
