#include "perftimer.h"
#include "logger.h"
#include "string.h"

//defaults set for production.
SafeLogger(perfTimer,false);
bool PerfTimer::showStarts = false;

PerfTimer::PerfTimer(TextKey note, bool enabled) :
  swatch(true),
  note(note),
  enabled(enabled){
  if(showStarts) {
    perfTimer("Starting timer %s", note);
  }
}

void PerfTimer::elapsed(){
  double seconds = swatch.elapsed();
  if(seconds<0) {
    perfTimer("!negative elapsed time");
  }
  if(enabled) {
    perfTimer("Elapsed time %s: %g", TextKey(note), seconds);
  }
}

PerfTimer::~PerfTimer(){
  elapsed();
}
