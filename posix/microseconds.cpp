//"(C) Andrew L. Heilveil, 2017"
#include "microseconds.h"


/* a value so large it won't be encountered in real life. (~136 years) */
const MicroSeconds MicroSeconds::Never(~0, ~0);

bool MicroSeconds::operator >(const MicroSeconds &that) const {
  return (raw.tv_sec > that.raw.tv_sec) || ((raw.tv_sec == that.raw.tv_sec) && (raw.tv_usec > that.raw.tv_usec));
}

bool MicroSeconds::operator >=(const MicroSeconds &that) const {
  return raw.tv_sec > that.raw.tv_sec || ((raw.tv_sec == that.raw.tv_sec) && raw.tv_usec >= that.raw.tv_usec);
}

bool MicroSeconds::operator ==(const MicroSeconds &that) const {
  return raw.tv_usec == that.raw.tv_usec && raw.tv_sec == that.raw.tv_sec; //test ns first as they change more frequently
}

MicroSeconds &MicroSeconds::operator +=(const MicroSeconds &lesser) {
  raw.tv_usec += lesser.raw.tv_usec;
  raw.tv_sec += lesser.raw.tv_sec;
  if (raw.tv_usec > OneMeg) { //since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    raw.tv_usec -= OneMeg;
    ++raw.tv_sec;
  }
  return *this;
}

MicroSeconds MicroSeconds::operator +(const MicroSeconds &lesser) const {
  MicroSeconds diff( *this);
  diff += lesser;
  return diff;
}

MicroSeconds &MicroSeconds::operator -=(const MicroSeconds &lesser) {
  raw.tv_usec -= lesser.raw.tv_usec;
  raw.tv_sec -= lesser.raw.tv_sec;
  if (raw.tv_usec < 0) { //since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    raw.tv_usec += OneMeg;
    --raw.tv_sec;
  }
  return *this;
}

MicroSeconds MicroSeconds::operator -(const MicroSeconds &lesser) const {
  MicroSeconds diff (*this);
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
  if (*this < other) {// NOLINT(*-branch-clone) leave as separate checks for debug
    *this = other;
  } else if (isNever()) {
    *this = other;
  }
  return *this;
}

MicroSeconds &MicroSeconds::atMost(const MicroSeconds &other) {
  if (isNever()) {//NOLINT(*-branch-clone) leave as separate checks for debug
    *this = other;
  } else if (*this > other) {
    *this = other;
  }
  return *this;
}
