#ifndef MICROSECONDS_H
#define MICROSECONDS_H

#include "time.h"

constexpr double from(const timeval &ts){
  return ts.tv_sec+1e-6*ts.tv_usec;
}

void parseTime(timeval &ts,double seconds);

struct MicroSeconds: public timeval{

  MicroSeconds(double seconds=0.0){
    this->operator= (seconds);
  }

  void operator=(double seconds){
    parseTime (*this,seconds);
  }

  double asSeconds()const{
    return from(*this);
  }

  operator double()const {
    return asSeconds();
  }

};

#endif // MICROSECONDS_H
