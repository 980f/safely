//"(C) Andrew L. Heilveil, 2017"
#include "microseconds.h"



const MicroSeconds MicroSeconds::Never(~0, ~0);

bool MicroSeconds::operator >(const MicroSeconds &that) const {
  return (tv_sec > that.tv_sec) || ((tv_sec == that.tv_sec) && (tv_usec > that.tv_usec));
}

bool MicroSeconds::operator >=(const MicroSeconds &that) const {
  return tv_sec > that.tv_sec || ((tv_sec == that.tv_sec) && tv_usec >= that.tv_usec);
}

bool MicroSeconds::operator ==(const MicroSeconds &that) const {
  return this->tv_usec == that.tv_usec && this->tv_sec == that.tv_sec; //test ns first as they change more frequently
}

MicroSeconds &MicroSeconds::operator +=(const MicroSeconds &lesser) {
  tv_usec += lesser.tv_usec;
  tv_sec += lesser.tv_sec;
  if (tv_usec > OneMeg) { //since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    tv_usec -= OneMeg;
    ++tv_sec;
  }
  return *this;
}

MicroSeconds MicroSeconds::operator +(const MicroSeconds &lesser) const {
  MicroSeconds diff;
  diff = *this;
  diff += lesser;
  return diff;
}

MicroSeconds &MicroSeconds::operator -=(const MicroSeconds &lesser) {
  tv_usec -= lesser.tv_usec;
  tv_sec -= lesser.tv_sec;
  if (tv_usec < 0) { //since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    tv_usec += OneMeg;
    --tv_sec;
  }
  return *this;
}

MicroSeconds MicroSeconds::operator -(const MicroSeconds &lesser) const {
  MicroSeconds diff;
  diff = *this;
  diff -= lesser;
  return diff;
}

/* this implementation is optimized for returns of 0 and 1 and presumes non-negative this and positive interval */
unsigned MicroSeconds::modulated(const MicroSeconds &interval) {
  if (isNever() || interval.isZero()) {
    return 0; //gigo
  }
  unsigned cycles = 0;
  //we use repeated subtraction to do a divide since most times we cycle 0 or 1.
  while (*this >= interval) {
    *this -= interval;
    ++cycles;
  }
  return cycles;
} // MicroSeconds::modulated


MicroSeconds &MicroSeconds::atLeast(const MicroSeconds &other) {
  if (*this < other) {
    *this = other;
  } else if (isNever()) {
    *this = other;
  }
  return *this;
}

MicroSeconds &MicroSeconds::atMost(const MicroSeconds &other) {
  if (isNever()) {
    *this = other;
  } else if (*this > other) {
    *this = other;
  }
  return *this;
}
