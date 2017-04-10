#ifndef SOFTPWM_H
#define SOFTPWM_H

#include "polledtimer.h"

/**
 * this could have been implemented with two PolledTimer's triggering each other, but that uses more critical resources than coding it directly.
 */
class SoftPWM: public PolledTimer {
protected:
  Ticks pair[2];//indices: low=0, high=1
  bool phase;

  /** we override the 'count complete' event and switch to the other interval time value */
  void onDone(void) override;
public:
  operator bool() const {
    return phase;
  }

  /** start running.
   * todo: add boolean to stop running instead of starting.
*/
  void trigger();

  void setPhase(bool highness,Ticks ticks);

  void setCycle(Ticks low, Ticks high);

  SoftPWM(Ticks low=0, Ticks high=0, bool andStart = false);

public:
  //since onDone is virtual we make this virtual also. We should try to replace that with a function member.
  virtual void onToggle(bool /*on*/){
    // a hook, overload with a pin toggle to make a PWM output.
  }

};


#endif // SOFTPWM_H
