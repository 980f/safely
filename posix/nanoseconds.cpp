#include "nanoseconds.h"
#include "minimath.h"

void parseTime(timespec &ts, double seconds){
  int wholeSeconds = splitter(seconds);
  ts.tv_sec = wholeSeconds;
  ts.tv_nsec = u32(1e9 * seconds);
}

void NanoSeconds::setMillis(unsigned ms){
  ts.tv_sec=ms/1000;//truncating divide
  ts.tv_nsec=(ms%1000)*1000000;
}
