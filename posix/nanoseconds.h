#ifndef NANOSECONDS_H
#define NANOSECONDS_H

#include <time.h>
/**
wrapper around timespec struct
*/


constexpr double from(const timespec &ts){
  return ts.tv_sec+1e-9*ts.tv_nsec;
}

void parseTime(timespec &ts,double seconds);

struct NanoSeconds {
  timespec ts;

  NanoSeconds(double seconds=0.0){
    this->operator= (seconds);
  }

  void operator=(double seconds){
    parseTime (ts,seconds);
  }

  operator double()const{
    return from(ts);
  }

  operator timespec &(){
    return ts;
  }

  void setMillis(unsigned ms);

};

#endif // NANOSECONDS_H
