#include "stopwatch.h"
#include "cheaptricks.h"

__time_t StopWatch::epoch = 0;

void StopWatch::readit(timespec &ts){
  clock_gettime(CLOCK_something,&ts);
  ts.tv_sec-=epoch;
}

StopWatch::StopWatch(bool beRunning,bool realElseProcess) :
  CLOCK_something(realElseProcess ? CLOCK_MONOTONIC : CLOCK_THREAD_CPUTIME_ID){
  readit(started);
  if(epoch==0) {//once per application start
    epoch = take(started.ts.tv_sec);
  }
  stopped = started;
  running = beRunning;
}

double StopWatch::roll(double *absolutely){
  double retval = elapsed(absolutely);//must be running to roll.
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

double StopWatch::absolute(){
  if(running) {
    readit(stopped);
  }
  return stopped;
}

unsigned StopWatch::cycles(double atHz){
  double seconds=elapsed();
  double events=seconds*atHz;
  return unsigned(events);//want truncation.
}

double StopWatch::elapsed(double *absolutely){
  double diff = absolute();
  if(absolutely) {
    *absolutely = diff;
  }
  diff -= started;
  if(diff<0) {//clock rolled over
    diff += 0.0;//todo:1 proper value before 2038 happens
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
