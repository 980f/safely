#ifndef SOFTPWM_H
#define SOFTPWM_H

#include "polledtimer.h"

/**
 * this could have been implemented with two PolledTimer's triggering each other, but that uses more critical resources than coding it directly.
 */
class SoftPWM: public PolledTimer {
protected:
  Ticks pair[2];//indices: is low=0, high=1
  bool phase;

  /** we override the 'count complete' event and switch to the other interval time value */
  void onDone(void) override {
    phase ^= 1;
    restart(pair[phase] - 1);//# the polledtimer stuff adds a 1 for good luck, we don't need no stinking luck. //todo: guard against a zero input
    onToggle(phase);
  }
public:
  operator bool() const {
    return phase;
  }

  void setPhase(bool highness,Ticks ticks){
    pair[highness] = ticks ?: 1;
  }

  void setCycle(Ticks low, Ticks high){
    pair[0] = low;
    pair[1] = high;
  }

  SoftPWM(Ticks low, Ticks high, bool andStart = false);

public:
  //since onDone is virtual we make this virtual also. We should try to replace that with a function member.
  virtual void onToggle(bool on){
    // a hook, overload with a pin toggle to make a PWM output.
  }

};


#endif // SOFTPWM_H
