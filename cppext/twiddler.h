#ifndef TWIDDLER_H
#define TWIDDLER_H
#include "eztypes.h"
/** software PWM
  * implements as a ratio of two integers without using division except during some of the optional setup functions.
  */

class IntegerTwiddler {
protected:
  int above;
  int below;
  int twiddle;
public:
  IntegerTwiddler();
  /** @see setRatio */
  IntegerTwiddler(u32 numer, u32 denom, bool center = true);

  /** numer out of (numer+denom) calls (in a row) to pwm() will return true. @param center true initializes the cycling to halfway through the longer part of the cycle.*/
  void setRatio(u32 numer, u32 denom, bool center = true);

  /** set ratio such that pwm is true once per @param rate calls
    * NB: rate of 0 makes pwm 'always true'*/
  void setDivider(int rate);
  /** rig for pwm() to always return @param on*/
  void freeze(bool on = false);

  /** @returns a bit the average of which is the ratio of the above and below members */
  bool pwm(void);

  operator bool (void){
    return pwm();
  }
};

/** add some convenient rate setting/getting functions, factored out of prior implementation to ge ta no-floating point base class */
class Twiddler:public IntegerTwiddler {
public:
  using IntegerTwiddler::IntegerTwiddler;
  using IntegerTwiddler::setRatio;//this was supposed to give access to setRatio(unsigned,unsigned,..._) but that quit working.
  void setRatio(double ratio);
  /**@return amount of time that twiddler is 'true' */
  double getRatio(void);
  /** @return 1/getRatio()*/
  double getDivider(void)const;

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
