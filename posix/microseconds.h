#ifndef MICROSECONDS_H
#define MICROSECONDS_H "(C) Andrew L. Heilveil, 2017"

#include "time.h"

constexpr double from(const timeval &ts){
  return ts.tv_sec+1e-6*ts.tv_usec;
}

void parseTime(timeval &ts,double seconds);

/** manages time as used by older POSIX systems */
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

  MicroSeconds &Never();

  bool isNever() const;

  bool isZero() const noexcept {
    return tv_sec==0 && tv_usec==0;
  }


public: //compares and math
  bool operator >(const MicroSeconds &that) const;
  bool operator >=(const MicroSeconds &that) const;
  bool operator ==(const MicroSeconds &that) const;
  unsigned modulated(const MicroSeconds &interval);
  MicroSeconds operator -(const MicroSeconds &lesser) const;
  MicroSeconds &operator -=(const MicroSeconds &lesser);
  MicroSeconds &operator +=(const MicroSeconds &lesser);
  MicroSeconds operator +(const MicroSeconds &lesser) const;
  MicroSeconds &atLeast(const MicroSeconds &other);
  MicroSeconds &atMost(const MicroSeconds &other);
};

#endif // MICROSECONDS_H
