#pragma once //"(C) Andrew L. Heilveil, 2017, github/980f"

#include "microseconds.h" //existed in Posix before nanoseconds so we convert just one way.
#include <ctime>
#include <minimath.h>
#include <type_traits>

/**
wrapper around timespec struct
*/

constexpr double from(const timespec &ts) {
  return ts.tv_sec + 1e-9 * ts.tv_nsec;
}


constexpr void parseTime(timespec &ts, double seconds){
  if(seconds==0.0) {//frequent case
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return;
  }

  ts.tv_sec = splitter(seconds);
  ts.tv_nsec = 1e9 * seconds;
}

struct NanoSeconds : timespec {

  constexpr NanoSeconds(double seconds) {
    this->operator=(seconds);
  }

  constexpr NanoSeconds(unsigned long long nanoseconds) {
    tv_nsec = nanoseconds;
    tv_sec = nanoseconds / 1'000'000'000;//truncating divide is desired
  }

  /** for timerfd we wish to create nonoseconds inline from a seconds value as double. */

  constexpr NanoSeconds(timespec ts) {
    tv_sec = ts.tv_sec;
    tv_nsec = ts.tv_nsec;
  }

  constexpr NanoSeconds() {
    tv_sec = 0;
    tv_nsec = 0;
  }

  constexpr NanoSeconds(const NanoSeconds &other) = default;

  constexpr NanoSeconds(const MicroSeconds us) {
    tv_sec = us.tv_sec;
    tv_nsec = 1000 * us.tv_usec;
  }


  template<typename Scalar> constexpr NanoSeconds &operator=(Scalar seconds) {
    if constexpr (std::is_same<timespec, Scalar>::value) {
      tv_sec = seconds.tv_sec;
      tv_nsec = seconds.tv_nsec;
    } else if constexpr (std::is_floating_point<Scalar>::value) {
      parseTime(*this, seconds);
    } else if constexpr (std::is_integral<Scalar>::value) {
      tv_sec = seconds;
      tv_nsec = 0;
    }
    return *this;
  }

  /** somewhat like a placement new, this adds NanoSeconds logic to an existing timespec */
  static NanoSeconds &wrap(timespec &embedded) {
    return *reinterpret_cast<NanoSeconds *>(&embedded);
  }

  operator double() const {
    return from(*this);
  }

  void setMillis(unsigned ms);

  /** @returns 0 if negative or 0 else ceiling of value*/
  unsigned ms() const noexcept;

  /** @returns microsecond where rounder=0 for truncation aka 'floor', 500 for nearest 'round', 999 for 'ceil'*/
  MicroSeconds us(unsigned rounder = 999);

  /** @returns whether this is zero */
  bool isZero() const noexcept;

  /** @returns whether this is definitely positive (>0) */
  bool inFuture() const;

  /** @returns this minus @param lesser. NB: this is not java, the object is on the stack not new'd */
  NanoSeconds operator -(const NanoSeconds &lesser) const;

  /** @returns this after subtracting @param lesser */
  NanoSeconds &operator -=(const NanoSeconds &lesser);

  /** @returns integer divide of this by interval, then sets this to remainder */
  unsigned modulated(const NanoSeconds &interval);

  /** @returns whether this comes after @param that */
  bool operator >(const NanoSeconds &that) const;

  /** @returns whether this comes after @param that */
  bool operator >=(const NanoSeconds &that) const;

  /** operator == doesn't make much sense, like for doubles */
  bool operator ==(const NanoSeconds &that) const;

  /** @returns 1,0,-1,  if @param dub is not null then it is the absolute value of the time in seconds. */
  int signabs(double *dub = nullptr) const;

  /** @return this after setting it to an invalidly large value. */
  NanoSeconds &Never();

  /** @returns whether this is the value that Never would set it to */
  bool isNever();

public: //logical operations that might surprise some people
  /** @returns this after setting it to other if other is larger. This only makes sense when computing a maximum interval, one that is large enough to contain all items presented to it. */
  NanoSeconds &atLeast(const NanoSeconds &other);

  /** @returns this after setting it to other if this is zero or other is smaller. This only makes sense when computing a minimum interval. */
  NanoSeconds &atMost(const NanoSeconds &other);

public: //system services
  /** wraps posix nanosleep. @returns the usual posix nonsense. 0 OK/-1 -> see errno
 sleeps for given amount, is set to time remaining if sleep not totally completed  */
  int sleep();

  /** wraps posix nanosleep. @returns the usual posix nonsense. 0 OK/-1 -> see errno
 if dregs is not null then it is set to dregs of nanosleep (amount by which the sleep returned early) */
  int sleep(NanoSeconds *dregs) const;

  NanoSeconds operator +(const NanoSeconds &lesser) const;

  NanoSeconds &operator +=(const NanoSeconds &lesser);
};
