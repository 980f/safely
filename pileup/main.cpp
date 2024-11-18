//(C) 2017 Andrew Heilveil


#include <iostream>
#include <random>
#include "minimath.h"


/** simulation of deadtime tracking amptek DP5 SCA signals on a tiny3.0 class controller */
class CounterIsr {
public:
  double quantum;//processor clock rate,  1 / Hz
  /** time from event to isr acknowledges it*/
  unsigned latency;
/** time from event to can interrupt again */
  unsigned cycle;

  /** stats on events converted and lost */
  unsigned events=0;
  unsigned rejected=0;
  unsigned detected=0;

  /** whether a count will generate an interrupt soon*/
  unsigned deferred=0;

  /** time left in period between last event and isr acknowldege, computed from latency */
  double vectoring=0;

  /** time left in period between last event and isr return/can interrupt again, computed from cycle */
  double storing=0;

  CounterIsr(unsigned latency=32, unsigned cycle=70,double MHz=96.0):
    quantum(1e-6/MHz),
    latency(latency),
    cycle(cycle){
    //#nada
  }

  /** event is latched in irq stuff, but not evaluted yet */
  void defer(){
    ++events;
    if(!deferred){
      ++deferred;
    } else {
      ++rejected;
    }
  }

  /** event lost */
  void lost(){
    ++events;
    ++rejected;
  }

  void detect(){
    ++events;
    ++detected; //one for the event that triggered the isr
    detected+=deferred; //and a retriggered one
    deferred=0;
  }

  /** compute timers */
  void trigger(){
    vectoring=quantum*latency;
    storing=quantum*cycle;
  }

  /** feed this the time since previous event */
  void next(double event){
    if(vectoring>0){
      if(event<vectoring){//occured before ack of previous
        vectoring-=event;//shift time base
        storing-=event;
        lost();
        return;
      }
      vectoring=0;
    }

    if(storing>0){
      if(event<storing){//still storing previous
        storing-=event;//shift time base
        defer();
        return;
      }
      if(event<storing+quantum*cycle){//still interacting with previous cycle
        //at event-storing we started vectoring again
        event-=storing;
        trigger();
        next(event);//recurse at most once or twice
        return;
      }
      storing=0;
    }

    detect();
    trigger();
  }

  double deadness()const{
    return ratio(100.0f*rejected,events);
  }

  double liveness()const{
    return ratio(100.0f*detected,events);
  }

  /** should be zero if code is correct */
  int sanitycheck()const{
    return rejected+detected-events;
  }
};

#include "poissonfeeder.h"

int main(int argc, char *argv[]){
  double cps=10000;
  unsigned latency=18;
  unsigned cycle=36;
  double MHz=96.0;
  printf("\nEstimate Isr pileup");
  while(argc-->0){
    double arg=atof(argv[argc]);
    switch (argc) {
    case 4:
      MHz=arg;
      break;
    case 3:
      cycle=unsigned(arg);
      break;
    case 2:
      latency=unsigned(arg);
      break;
    case 1: //rate
      cps=arg;
      break;
    }
  }


  CounterIsr sim(latency,cycle,MHz);
  PoissonFeeder pfeeder(cps);

  printf("\nArguments: cps:%g latency:%u  cycle:%u osc:%g ",cps,latency,cycle,MHz);

  /** run simulation */
  for(int trials=10000; trials-->0;){
    double randy=pfeeder();
    sim.next(randy);
  }

  printf("\nAfter %u trials dead time was %g%% ",sim.events,sim.deadness());
  printf("\nRaw counts: %u %u %u \n",sim.events,sim.detected,sim.rejected);
  return 0;
}
