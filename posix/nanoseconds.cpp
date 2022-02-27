//"(C) Andrew L. Heilveil, 2017"
#include "nanoseconds.h"
#include "minimath.h"

static const int OneGig = 1000000000;
static const int OneMeg = 1000000;

void parseTime(timespec &ts, double seconds){
  if(seconds==0.0) {//frequent case
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return;
  }

  ts.tv_sec = splitter(seconds);
  ts.tv_nsec = u32(1e9 * seconds);
}

void NanoSeconds::setMillis(unsigned ms){
  this->tv_sec = revolutions(ms,1000);//truncating divide
  this->tv_nsec = ms * OneMeg;
}

unsigned NanoSeconds::ms() const noexcept {
  if(signabs()<=0) {
    return 0;
  }
  unsigned millis = tv_sec;
  millis += quanta(tv_nsec,OneMeg);//round any fraction up.
  return millis;
}

MicroSeconds NanoSeconds::us(unsigned rounder){
  MicroSeconds us;
  us.tv_sec = tv_sec;
  us.tv_usec = (tv_nsec + rounder) / 1000;
  return us;
}

NanoSeconds& NanoSeconds::operator -=(const NanoSeconds &lesser){
  tv_nsec -= lesser.tv_nsec;
  tv_sec -= lesser.tv_sec;
  if(tv_nsec<0) {//since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    tv_nsec += OneGig;
    --tv_sec;
  }
  return *this;
}

NanoSeconds NanoSeconds::operator -(const NanoSeconds &lesser) const {
  NanoSeconds diff;
  diff = *this;
  diff -= lesser;
  return diff;
}

NanoSeconds& NanoSeconds::operator +=(const NanoSeconds &lesser){
  tv_nsec += lesser.tv_nsec;
  tv_sec += lesser.tv_sec;
  if(tv_nsec>OneGig) {//since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    tv_nsec -= OneGig;
    ++tv_sec;
  }
  return *this;
}

NanoSeconds NanoSeconds::operator +(const NanoSeconds &lesser) const {
  NanoSeconds diff;
  diff = *this;
  diff += lesser;
  return diff;
}

/* this implementation is optimized for returns of 0 and 1 and presumes non-negative this and positive interval */
unsigned NanoSeconds::modulated(const NanoSeconds &interval){
  if(interval.isZero()) {
    return 0;//gigo
  }
  unsigned cycles = 0;
  //we use repeated subtraction to do a divide since most times we cycle 0 or 1.
  while(*this>=interval) {
    *this -= interval;
    ++cycles;
  }
  return cycles;
} // NanoSeconds::modulated

bool NanoSeconds::operator >(const NanoSeconds &that) const {
  return (this->tv_sec>that.tv_sec) || ( (this->tv_sec==that.tv_sec)&&(this->tv_nsec>that.tv_nsec) );
}

bool NanoSeconds::operator >=(const NanoSeconds &that) const {
  return this->tv_sec>that.tv_sec ||((this->tv_sec==that.tv_sec) && this->tv_nsec>=that.tv_nsec);
}

bool NanoSeconds::operator ==(const NanoSeconds &that) const {
  return tv_nsec==that.tv_nsec && this->tv_sec == that.tv_sec;//test ns first as they change more frequently
}

int NanoSeconds::signabs(double *dub) const {
  if(tv_sec<0) {
    if(dub) {
      *dub = -from(*this);
    }
    return -1;
  }
  if(isZero()) {
    if(dub) {
      *dub = 0;
    }
    return 0;
  }
  if(dub) {
    *dub = from(*this);
  }
  return 1;
} // NanoSeconds::signabs

NanoSeconds &NanoSeconds::atLeast(const NanoSeconds &other){
  if(*this<other) {
    *this = other;
  } else if(isNever()) {
    *this = other;
  }
  return *this;
}

NanoSeconds &NanoSeconds::atMost(const NanoSeconds &other){
  if(isNever()) {
    *this = other;
  } else if(*this>other) {
    *this = other;
  }
  return *this;
}

NanoSeconds &NanoSeconds::Never(){
  this->tv_sec = ~0;//this is legal
  this->tv_nsec = ~0;//this is not.
  return *this;
}

bool NanoSeconds::isNever(){
  return this->tv_sec==~0 && this->tv_nsec==~0;
}

bool NanoSeconds::isZero() const noexcept {
  return this->tv_sec==0 && this->tv_nsec==0;
}

bool NanoSeconds::inFuture() const {
  return this->tv_sec>0 || (this->tv_sec==0 && this->tv_nsec>0);
}

int NanoSeconds::sleep(){
  return nanosleep(this, this);
}

int NanoSeconds::sleep(NanoSeconds *dregs) const {
  return nanosleep(this,dregs ? dregs : nullptr);
}
