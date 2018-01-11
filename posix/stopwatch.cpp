#include "stopwatch.h"
#include "cheaptricks.h"

bool StopWatch::readit(timespec &ts){
  if(clock_gettime(CLOCK_something,&ts)){
    return false;
  } else {
    ts.tv_sec-=epoch;
    return true;
  }
}

StopWatch::StopWatch(bool beRunning,bool realElseProcess) :
  CLOCK_something(realElseProcess ? CLOCK_MONOTONIC : CLOCK_THREAD_CPUTIME_ID){
  epoch=0;
  readit(started);
  epoch = take(started.tv_sec);
  stopped = started;
  running = beRunning;
}

void StopWatch::lap(const StopWatch &other){
  started=other.stopped;
  running = true;
}


NanoSeconds StopWatch::roll(double *absolutely){
  NanoSeconds retval = elapsed(absolutely);//must be running to roll.
  if(running) {
    started = stopped;//#do NOT start(), want to read the clock just once with each roll.
  }
  return retval;
}

void StopWatch::start(){
  readit(started);
  running = true;
}

void StopWatch::stop(){
  if(flagged(running)) {
    readit(stopped);
  }
}

bool StopWatch::isRunning() const {
  return running;
}

NanoSeconds StopWatch::absolute(){
  if(running) {
    readit(stopped);
  }
  return stopped;
}

unsigned StopWatch::cycles(double atHz,bool andRoll){
  double seconds=elapsed();
  //could provide for more precise cycling by adjusting start by fraction of events *1/atHz.
  double events=seconds*atHz;
  unsigned cycles=unsigned(events);//#we want truncation, rounding would be bad.
  if(andRoll){
    if(cycles>0){//only roll if the caller is going to take action.
      if(running) {
        started = stopped;//#do NOT start(), want to read the clock just once with each roll.
        // a refined version would subtract out the fractional part of events/atHz, for less jitter.
        //if we did that we could drop the test for cycles>0, however doing that test is efficient.
      }
    }
  }
  return cycles;
}

unsigned StopWatch::periods(NanoSeconds interval, bool andRoll){
  NanoSeconds now=elapsed();
  unsigned modulo=now.modulated(interval);
  if(andRoll){
    started=stopped-now;//now at this point is fraction of an interval
  }
  return modulo;
}

NanoSeconds StopWatch::lastSnap(bool absolute) const{
  if(absolute){
    return stopped;
  } else {
    return stopped-started;
  }
}

NanoSeconds StopWatch::elapsed(double *absolutely){
  NanoSeconds diff = absolute();//updates 'stopped'
  if(absolutely) {
    *absolutely = diff;
  }
  diff -= started;
  if(diff<0) {//clock rolled over
    diff.Never();//todo:1 proper value before 2038 happens
  }
  return diff;
}

void StopWatch::rollit(){
  if(running){
    started = stopped;//#do NOT start(), want to read the clock just once with each roll.
  } else {
    start();
  }
} // StopWatch::elapsed
