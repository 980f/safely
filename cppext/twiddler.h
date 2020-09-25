#pragma once
#define TWIDDLER_H  //(C) 2019 Andy Heilveil, github/980f

/** software PWM
  * implements as a ratio of two integers without using division except during some of the optional setup functions.
  *
  * Attempts to make the integer type a template parameter abandoned due to how annoying the syntax was.
  */


class IntegerTwiddler {
protected:
  unsigned above;
  unsigned below;
  int twiddle;
public:
  IntegerTwiddler();

  /** @see setRatio */
  IntegerTwiddler(unsigned numer, unsigned denom, bool center = true);

  /** numer out of (numer+denom) calls (in a row) to pwm() will return true. @param center true initializes the cycling to halfway through the longer part of the cycle.*/
  void setRatio(unsigned numer, unsigned denom, bool center = true);

  /** set ratio such that pwm is true once per @param rate calls
    * NB: rate of 0 makes pwm 'always true'*/
  void setDivider(unsigned rate);

  /** rig for pwm() to always return @param on*/
  void freeze(bool on = false);

  /** @returns a bit the average of which is the ratio of the above and below members */
  bool pwm(void);

  operator bool(void) {
    return pwm();
  }
};

/** add some convenient rate setting/getting functions, to keep the base class free of floating point */
class Twiddler : public IntegerTwiddler {
public:
  void setRatio(double ratio);

  /**@return amount of time that twiddler is 'true' */
  double getRatio(void);

  /** @return 1/getRatio()*/
  double getDivider(void) const;
};

/** pulse width modulator modulator (not redundant!)
  * modulates a value intended for use in pwm signal to
  * average to a given value */
class PwmModulator : public Twiddler {
  unsigned quoticks; //subtract or add this amount per tick for accel/decel respectively
public:
  PwmModulator();

  void setRatio(double ratio);

  double getRatio(void);

  /** this intentionally hides the IntegerTwiddler bool pwm() method, do not make a virtual hierarchy, rename if it bothers you.*/
  unsigned pwm(void);

  /** this value will vary such that its average will approach the ratio.*/
  operator unsigned(void) {
    return pwm();
  }

  /** noFloat gives a rougher estimate but doesn't use floating point, relevent to an ISR*/
  unsigned quanta(unsigned chunk, bool noFloat = true);
};

