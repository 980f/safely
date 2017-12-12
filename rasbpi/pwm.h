#ifndef PWM_H
#define PWM_H

#include "peripheral.h"

/**
 * The underlying hardware is either a PWM or a pattern generator.
 * This module only supports PWM use.
 *
 * There are a pair of devices, which share pattern generator hardware and a system clock.
 */

/** A hardware pwm output. The shared part of the logic is static in the cpp file. */
class Pwm {
  friend class PwmControl;
  bool pwm1;//there can only be two so use a boolean

  Pwm(bool pwm1);
public:
  static Pwm *get(bool pwm1);
  //base frequency is set by a clock.
  static void setSharedClock(double hz);

  /***/
  void configure(bool lowActive);

  /** the denominator sets the update rate.
  numerator/denominator is the duty cycle
if numerator>denominator then the output stays perpetually high */
  void setRatio(unsigned leading, unsigned total);

  void enable(bool onElseOff)const ;
  void run() const {
    enable(true);
  }
  void stop()const {
    enable(false);
  }

};

#endif // PWM_H
