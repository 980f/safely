#include <iostream>
#include <random>
#include "minimath.h"

class CounterIsr {
public:
  double quantum;//1 osc MHz
  /** time from event to isr acknowledges it*/
  unsigned latency;
/** time from event to can interrupt again */
  unsigned cycle;

  unsigned events=0;
  unsigned rejected=0;
  unsigned detected=0;

  unsigned deferred=0;

  double vectoring=0;
  double storing=0;
  double restarting=0;

  CounterIsr(unsigned latency=32, unsigned cycle=70,double MHz=96.0):
    quantum(1e-6/MHz),
    latency(latency),
    cycle(cycle){

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

  void lost(){
    ++events;
    ++rejected;
  }

  void detect(bool clean){
    ++events;
    if(clean){
      ++detected;
    }
    detected+=deferred;
    deferred=0;
  }

  void trigger(){
    vectoring=quantum*latency;
    storing=quantum*cycle;
  }

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
      //at event-storing we started vectoring again
      event-=storing;
      trigger();
      next(event);//recurse at most once or twice
      return;
    }

    detect(true);
    trigger();
  }

  double deadness()const{
    return rate(rejected,events);
  }

  double liveness()const{
    return rate(detected,events);
  }

  int sanitycheck()const{
    return rejected+detected-events;
  }
};


using namespace std;

int main(int argc, char *argv[])
{
  double cps=10000;
  unsigned latency=32;
  unsigned cycle=70;
  double MHz=96.0;
  cout << "\nEstimate Isr pileup" << endl;
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

  std::random_device rd;
  std::mt19937 rnd_gen (rd ());

//  uniform_real_distribution<double> uniform(0.0,1.0);
  exponential_distribution<double> generator(ratio(1.0,cps));

  printf("\nArguments: cps:%g latency:%u  cycle:%u osc:%g ",cps,latency,cycle,MHz);
  for(int trials=100; trials-->0;){
    double randy=generator(rnd_gen);
    sim.next(randy);
  }

  printf("\nAfter %u trials dead time was %g%% ",sim.events,sim.deadness());
  return 0;
}
