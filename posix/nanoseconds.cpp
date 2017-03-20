#include "nanoseconds.h"

//all inlined at the moment

void parseTime(timespec &ts, double seconds){
  ts.tv_sec=seconds;//truncate
  ts.tv_nsec=1e9*(seconds-ts.tv_sec);
}
