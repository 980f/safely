#ifndef TWIDDLER_H
#define TWIDDLER_H
#include "eztypes.h"
/** software PWM
  * implements as a ratio of two integers without using division except during initialization.
  */

class Twiddler {
  int above;
  int below;
  int twiddle;
public:
  Twiddler();
  /** @see setRatio */
  Twiddler(u32 numer, u32 denom, bool center = true);
  /** numer out of (numer+denom) calls (in a row) to pwm() will return true. @param center true initializes the cycling to halfway through the longer part of the cycle.*/
  void setRatio(u32 numer, u32 denom, bool center = true);
  void setRatio(double ratio);
  /**@return amount of time that twiddler is 'true' */
  double getRatio(void);
  /** @return 1/getRatio()*/
  double getDivider(void)const;

  /** set ratio such that pwm is true once per @param rate calls
    * NB: rate of 0 makes pwm 'always true'*/
  void setDivider(int rate);
  /** rig for pwm() to always return @param on*/
  void freeze(bool on = false);
  bool pwm(void);
  operator bool (void){
    return pwm();
  }
  //unsigned fractionOf(unsigned channel) ~ channel * (twiddle/(above+below))
};

/** pulse width modulator modulator (not redundant!)
  * moddulates a value intended for use in pwm signal to
  * average to a given value */
class PwmModulator : public Twiddler {
  unsigned quoticks; //subtract or add this amount per tick for accel/decel respectively
public:
  PwmModulator();
  void setRatio(double ratio);
  double getRatio(void);
  unsigned pwm(void);
  /** this value will vary such that its average will approach the ratio.*/
  operator unsigned(void) {
    return pwm();
  }
  /** noFloat gives a rougher estimate but doesn't use floating point, relevent to an ISR*/
  unsigned quanta(unsigned chunk,bool noFloat=true);
};

#endif // TWIDDLER_H
