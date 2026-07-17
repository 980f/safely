//"(C) Andrew L. Heilveil, 2017"
#include "nanoseconds.h"
#include "minimath.h"


static const int OneMeg = 1000000;


void NanoSeconds::setMillis(unsigned ms){
  raw.tv_sec = revolutions(ms,1000);//truncating divide
  raw.tv_nsec = ms * OneMeg;
}

unsigned NanoSeconds::ms() const noexcept {
  if(signabs()<=0) {
    return 0;
  }
  unsigned millis = raw.tv_sec * 1000; //*1000 was missing for a while!
  millis += quanta(raw.tv_nsec,OneMeg);//round any fraction up.
  return millis;
}

MicroSeconds NanoSeconds::us(unsigned rounder){
  return MicroSeconds {static_cast<unsigned>(raw.tv_sec),static_cast<unsigned>((raw.tv_nsec + rounder) / 1000)};
}

NanoSeconds& NanoSeconds::operator -=(const NanoSeconds &lesser){
  raw.tv_nsec -= lesser.raw.tv_nsec;
  raw.tv_sec -= lesser.raw.tv_sec;
  if(raw.tv_nsec<0) {//since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    raw.tv_nsec += Billion;
    --raw.tv_sec;
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
  raw.tv_nsec += lesser.raw.tv_nsec;
  raw.tv_sec += lesser.raw.tv_sec;
  if(raw.tv_nsec>Billion) {//since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    raw.tv_nsec -= Billion;
    ++raw.tv_sec;
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
  return (raw.tv_sec>that.raw.tv_sec) || ( (raw.tv_sec==that.raw.tv_sec)&&(raw.tv_nsec>that.raw.tv_nsec) );
}

bool NanoSeconds::operator >=(const NanoSeconds &that) const {
  return raw.tv_sec>that.raw.tv_sec ||((raw.tv_sec==that.raw.tv_sec) && raw.tv_nsec>=that.raw.tv_nsec);
}

bool NanoSeconds::operator ==(const NanoSeconds &that) const {
  return raw.tv_nsec==that.raw.tv_nsec && raw.tv_sec == that.raw.tv_sec;//test ns first as they change more frequently
}

int NanoSeconds::signabs(double *dub) const {
  if(raw.tv_sec<0) {
    if(dub) {
      *dub = -from(raw);
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
    *dub = from(raw);
  }
  return 1;
} // NanoSeconds::signabs

NanoSeconds &NanoSeconds::atLeast(const NanoSeconds &other){//todo:1 named function instead of operator
  if(*this<other) { // NOLINT(*-branch-clone) leave separate for debug
    *this = other;
  } else if(isNever()) {
    *this = other;
  }
  return *this;
}

NanoSeconds &NanoSeconds::atMost(const NanoSeconds &other){//todo:1 named function instead of operator
  if(isNever()) { // NOLINT(*-branch-clone) leave separate for debug
    *this = other;
  } else if(*this>other) {
    *this = other;
  }
  return *this;
}

NanoSeconds &NanoSeconds::Never(){
  raw.tv_sec = ~0;//this is legal
  raw.tv_nsec = ~0;//this is not.
  return *this;
}

bool NanoSeconds::isNever() const noexcept {
  return raw.tv_sec==~0 && raw.tv_nsec==~0;
}

bool NanoSeconds::isZero() const noexcept {
  return raw.tv_sec==0 && raw.tv_nsec==0;
}

bool NanoSeconds::inFuture() const {
  return raw.tv_sec>0 || (raw.tv_sec==0 && raw.tv_nsec>0);
}

int NanoSeconds::sleep(){
  return nanosleep(&raw, &raw);
}

int NanoSeconds::sleep(NanoSeconds *dregs) const {
  return nanosleep(&raw,dregs ? &dregs->raw : nullptr);
}
