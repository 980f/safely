#include "performancelogger.h"

#include <textformatter.h>

PerformanceLogger::PerformanceLogger(TextKey format, Fildes &replyto):
  perf(true,false),
  format(format),
  replyto(replyto){
  //#nada
}

PerformanceLogger::~PerformanceLogger(){
  onDone();
}

void PerformanceLogger::onDone(){
  perf.stop();
  TextFormatter smsg(format);
  smsg.apply(perf.elapsed()*1000);
  replyto.write(smsg.asBytes());
}
