#ifndef NANOSECONDS_H
#define NANOSECONDS_H "(C) Andrew L. Heilveil, 2017"

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

  NanoSeconds(const NanoSeconds &other)=default;\

  NanoSeconds& operator=(double seconds){
    parseTime (ts,seconds);
    return *this;
  }

  //allow default assignment to work

  operator double()const{
    return from(ts);
  }

  operator timespec &(){
    return ts;
  }

  void setMillis(unsigned ms);

  /** @returns 0 if negative or 0 else ceiling of value*/
  unsigned ms() const noexcept;
  NanoSeconds operator -(const NanoSeconds &lesser) const;
  NanoSeconds &operator -=(const NanoSeconds &lesser);

  /** @returns whether this comes after @param that */
  NanoSeconds operator >(const NanoSeconds &that);

  /** @returns 1,0,-1,  if @param dub is not null then it is the absolute value of the time in seconds. */
  int signabs(double *dub=nullptr)const;

  bool isZero()const noexcept;


  /** wraps posix nanosleep. @returns the usual posix nonsense. 0 OK/-1 -> see errno
 sleeps for given amount, is set to time remaining if sleep not totally completed  */
  int sleep();

  /** wraps posix nanosleep. @returns the usual posix nonsense. 0 OK/-1 -> see errno
 if dregs is not null then it is set to dregs of nanosleep (amount by which the sleep returned early) */
  int sleep(NanoSeconds *dregs)const;

};

#endif // NANOSECONDS_H
