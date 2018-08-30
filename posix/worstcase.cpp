#include "worstcase.h"

#include "logger.h"

SafeLogger(worstCase,true);//default on as it isn't very noisy once things settled down.

void WorstCase::mark(){
  watcher.start();
}

void WorstCase::show(){
  if(latency.inspect(watcher.elapsed())) {
    worstCase(format.c_str(),1000.0*latency.max());//ms for legibility
  }
}

void WorstCase::lap(const WorstCase &other){
  watcher.lap(other.watcher);
}

WorstCase::WorstCase(TextKey fmt, bool realElseProcess):
  watcher(false,realElseProcess),
  format(fmt){
  //#done
}

void WorstCase::Chain(WorstCase **scanner){
  if(scanner){
    if(*scanner){
      lap(**scanner);
    } else {
      mark();
    }
    *scanner=this;
  }
}

WorstCase::AutoShow WorstCase::time(){
  return AutoShow(*this);
}
