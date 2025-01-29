# pragma once // "(C)  2017 Andrew L. Heilveil, aka github/980f"

#include "minimath.h"
#include <ctime>


/** manages time as used by older POSIX systems */
struct MicroSeconds : timeval {
  static const decltype(tv_usec) OneMeg = 1000000;


  static constexpr double from(const timeval &ts) {
    return ts.tv_sec + (1.0/OneMeg) * ts.tv_usec; //ignores possibility of tv_sec being very large.
  }

  static constexpr void parseTime(timeval &ts, double seconds) {
    if (seconds == 0.0) { //frequent case
      ts.tv_sec = 0;
      ts.tv_usec = 0;
      return;
    }
    ts.tv_sec = splitter(seconds);//splitter modifies its operand to be a fraction
    ts.tv_usec = round(OneMeg * seconds);
  }

  /* a floating point value is number of seconds */
  constexpr explicit MicroSeconds(double seconds) {
    this->operator=(seconds);
  }


  /** two integers is explicit seconns and micros */
  constexpr MicroSeconds(unsigned sec, unsigned micro): timeval{sec, micro} {} //still getting familiar with {} instead of () init, looks weird!

  constexpr MicroSeconds &operator=(double seconds) {
    parseTime(*this, seconds);
    return *this;
  }

  template<typename Scalar> constexpr MicroSeconds &operator=(Scalar seconds) {
    if constexpr (std::is_same<timeval, Scalar>::value) {
      tv_sec = seconds.tv_sec;
      tv_usec = seconds.tv_usec;
    } else if constexpr (std::is_same<timespec, Scalar>::value) {
      tv_sec = seconds.tv_sec;
      tv_usec = seconds.tv_nsec/1000;
    } else if constexpr (std::is_floating_point<Scalar>::value) {
      parseTime(*this, seconds);
    } else if constexpr (std::is_integral<Scalar>::value) {
      tv_usec = seconds;
      tv_sec = seconds / OneMeg; //truncating divide is desired
    }
    return *this;
  }

  /** a single integer argymentis number of microseconds */
  //template is being used to resolve some "ambiguous overloads".
  template<typename Integrish> constexpr MicroSeconds(Integrish microseconds) {
    this->operator=(microseconds);
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
    return tv_sec == ~0; //no longer check usec, it is so easily clipped by OneMeg // && tv_usec == ~0;
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
