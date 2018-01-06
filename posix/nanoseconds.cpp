//"(C) Andrew L. Heilveil, 2017"
#include "nanoseconds.h"
#include "minimath.h"

void parseTime(timespec &ts, double seconds){
  if(seconds==0.0){//frequent case
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return;
  }

  ts.tv_sec = splitter(seconds);
  ts.tv_nsec = u32(1e9 * seconds);
}

void NanoSeconds::setMillis(unsigned ms){
  ts.tv_sec=ms/1000;//truncating divide
  ts.tv_nsec=(ms%1000)*1000000;
}

unsigned NanoSeconds::ms() const noexcept {
  if(signabs()<=0){
    return 0;
  }
  unsigned millis=ts.tv_sec;
  millis+= quanta(ts.tv_nsec,1000000);//round any fraction up.
  return millis;
}

NanoSeconds NanoSeconds::operator -(const NanoSeconds &lesser) const{
  NanoSeconds diff;
  diff=*this;
  diff-=lesser;
  return diff;
}

NanoSeconds& NanoSeconds::operator -=(const NanoSeconds &lesser){
  ts.tv_nsec-=lesser.ts.tv_nsec;
  ts.tv_sec-=lesser.ts.tv_sec;
  if(ts.tv_nsec<0){//since both values are at most 999999999 their difference is <= -999999999, we have enough spare bits to serve as a carry out.
    ts.tv_nsec+=1000000000;
    --ts.tv_sec;
  }
  return *this;
}

/* this implementation is optimized for returns of 0 and 1 and presumes non-negative this and positve interval */
unsigned NanoSeconds::modulated(const NanoSeconds &interval){
  if(interval.isZero()){
    return 0;//gigo
  }
  unsigned cycles=0;
  while(*this>=interval){
    *this-=interval;
    ++cycles;
  }
  return cycles;
}

bool NanoSeconds::operator >(const NanoSeconds &that) const{
  return (ts.tv_sec>that.ts.tv_sec) || ( (ts.tv_sec==that.ts.tv_sec)&&(ts.tv_nsec>that.ts.tv_nsec) );
}

bool NanoSeconds::operator >=(const NanoSeconds &that) const{
  return (ts.tv_sec>=that.ts.tv_sec) || ( (ts.tv_sec==that.ts.tv_sec)&&(ts.tv_nsec>=that.ts.tv_nsec) );
}

int NanoSeconds::signabs(double *dub) const {
  if(ts.tv_sec<0){
    if(dub){
      *dub=-from(ts);
    }
    return -1;
  }
  if(ts.tv_sec==0 && ts.tv_nsec==0){
    if(dub){
      *dub=0;
    }
    return 0;
  }
  if(dub){
    *dub=from(ts);
  }
  return 1;
}

bool NanoSeconds::isZero() const noexcept{
  return ts.tv_sec==0 && ts.tv_nsec==0;
}

int NanoSeconds::sleep(){
  return nanosleep(&ts, &ts);
}

int NanoSeconds::sleep(NanoSeconds *dregs)const{
  return nanosleep(&ts,dregs?&dregs->ts:nullptr);
}
