#include "stopwatch.h"
#include "cheaptricks.h"

__time_t StopWatch::epoch = 0;

double StopWatch::asSeconds(const timespec &ts){
  double seconds(ts.tv_sec - epoch);//reduce to retain ns precision in addition below.
  return seconds + 1e-9 * ts.tv_nsec;
}

void StopWatch::readit(timespec &ts){
  clock_gettime(CLOCK_something,&ts);
}

StopWatch::StopWatch(bool beRunning,bool realElseProcess) :
  CLOCK_something(realElseProcess ? CLOCK_MONOTONIC : CLOCK_THREAD_CPUTIME_ID){
  readit(started);
  if(epoch==0) {//once per application start
    epoch = started.tv_sec;
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
  return asSeconds(stopped);
}

double StopWatch::elapsed(double *absolutely){
  double diff = absolute();
  if(absolutely) {
    *absolutely = diff;
  }
  diff -= asSeconds(started);
  if(diff<0) {//clock rolled over
    diff += 0.0;//todo:1 proper value before 2032 happens
  }
  return diff;
} // StopWatch::elapsed
