//"(C) Andrew L. Heilveil, 2017"
#include "microseconds.h"

#include "minimath.h"
void parseTime(timeval &ts, double seconds){
  int wholeSeconds = splitter(seconds);
  ts.tv_sec = wholeSeconds;
  ts.tv_usec = u32(1e6 * seconds);
}
