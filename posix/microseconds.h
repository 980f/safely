# pragma once // "(C)  2017 Andrew L. Heilveil, aka github/980f"

#include "minimath.h"
#include <ctime>

constexpr double from(const timeval &ts) {
  return ts.tv_sec + 1e-6 * ts.tv_usec; //ignores possibility of tv_sec being very large.
}

constexpr void parseTime(timeval &ts, double seconds) {
  if (seconds == 0.0) { //frequent case
    ts.tv_sec = 0;
    ts.tv_usec = 0;
    return;
  }
  ts.tv_sec = splitter(seconds);
  ts.tv_usec = unsigned(1e6 * seconds); //splitter modifies the operand to be a fraction
}

/** manages time as used by older POSIX systems */
struct MicroSeconds : timeval {
  static const int OneMeg = 1000000;

  constexpr explicit MicroSeconds(double seconds = 0.0) {
    this->operator=(seconds);
  }

  constexpr MicroSeconds(unsigned sec, unsigned micro): timeval{0, 0} {} //still getting familiar with {} instead of () init, looks weird!

  constexpr MicroSeconds(unsigned long long microseconds) : MicroSeconds(microseconds / OneMeg, microseconds % OneMeg) {}

  constexpr MicroSeconds &operator=(double seconds) {
    parseTime(*this, seconds);
    return *this;
  }

  constexpr double asSeconds() const {
    return from(*this);
  }

  constexpr operator double() const {
    return asSeconds();
  }

  /* a practically unachievable value, a Nan for this type */
  static const MicroSeconds Never;

  /** @returns whether this microsecond is 'never', an unachievable value, a Nan for this type */
  constexpr bool isNever() const {
    return tv_sec == ~0 && tv_usec == ~0;
  }

  constexpr bool isZero() const noexcept {
    return tv_sec == 0 && tv_usec == 0;
  }

  /** @returns whether this is neither never, nor zero, but it might be in the past if an absolute time value */
  constexpr bool isReal() const noexcept {
    return !isZero() && !isNever();
  }

public: //compares and math
  /** beware that Never is always greater than anything other than Never */
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
