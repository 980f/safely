#include "perftimer.h"
#include "logger.h"
#include "string.h"


bool PerfTimer::all=true;
//# only available via debugger:
bool showStarts=false;

PerfTimer::PerfTimer(TextKey note, bool enabled):
  swatch(true),
  note(note),
  enabled(enabled) {
  if(enabled&&showStarts){
    dbg("Starting timer %s", note);
  }
}

void PerfTimer::elapsed(){
  double seconds=swatch.elapsed();
  if(seconds<0){
    wtf("!negative elapsed time");
  }
  if(PerfTimer::all&&enabled){
    dbg("Elapsed time %s: %g", TextKey(note), seconds);
  }
}

PerfTimer::~PerfTimer(){
  elapsed();
}
