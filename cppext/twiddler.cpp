#include "twiddler.h"
#include "minimath.h"

#ifdef UseContinuedFractions
#include "continuedfractionratiogenerator.h"
#endif

IntegerTwiddler::IntegerTwiddler() {
  freeze();
}

IntegerTwiddler::IntegerTwiddler(unsigned numer, unsigned denom, bool center) {
  setRatio(numer, denom, center);
}

//neither following pragma worked :(  suppressed via CLI arg for the whole file.
//#pragma clang diagnostic ignored "-Wsign-compare"
//#pragma gcc diagnostic ignored "-Wsign-compare"
void IntegerTwiddler::setRatio(unsigned numer, unsigned denom, bool center) {
  above = numer;
  below = denom;
  if (center) {
    twiddle = (above - below) / 2;
  } else {
    if (twiddle > below) {  //#-Wsign-compare OK
      twiddle = below;
    } else if (twiddle < -above) { //#-Wsign-compare OK
      twiddle = -above;
    }
  }
} /* setRatio */


void Twiddler::setRatio(double ratio) {
  if (ratio <= 0) {
    freeze(false);
    return;
  }

#ifdef UseContinuedFractions
  ContinuedFractionRatioGenerator cfrg=ContinuedFractionRatioGenerator::Run(ratio);//Note: this might do 30 or so floating point divides.
  IntegerTwiddler::setRatio(cfrg.numerator(),cfrg.denominator());//todo: test if we have the right order here, might need to swap the operands.
#else
//a value used to get the maximum precision/resolution available due to the data type of twiddle
  static const unsigned MaxRes = ~0U>>1; //todo:3 max pos int, find standard symbol for it.

  if (ratio >= 1.0) {
    IntegerTwiddler::setRatio(MaxRes * ratio, MaxRes);
  } else {
    IntegerTwiddler::setRatio(MaxRes, MaxRes * ratio);
  }
#endif
}

double Twiddler::getRatio() {
  return ratio(double(above), double(below)); //use double so as to not lose accuracy on ints near to max.
}

double Twiddler::getDivider() const {
  if (below == 0) {
    return 0.0;//don't trust ratio(0,0) to stay the same over the ages :)
  }
  return ratio(double(below), double(above));
}

void IntegerTwiddler::setDivider(unsigned rate) {
  if (rate == 0) {
    freeze(false);
  } else {
    above = 1;
    below = rate;
    twiddle = -rate / 2;
  }
}

void IntegerTwiddler::freeze(bool on) {
  above = 0;
  below = 0;
  twiddle = on - 1; //#hack
}

bool IntegerTwiddler::pwm(void) {
  if (twiddle >= 0) {
    twiddle -= below;
    return true;
  } else {
    twiddle += above;
    return false;
  }
}

/////////////////////
PwmModulator::PwmModulator() {
  quoticks = 0;
}

void PwmModulator::setRatio(double ratio) {
  quoticks = unsigned(ratio); //#truncation desired
  Twiddler::setRatio(ratio - quoticks);
}

double PwmModulator::getRatio(void) {
  return Twiddler::getRatio() + quoticks;
}

unsigned PwmModulator::pwm(void) {
  return quoticks + Twiddler::pwm();
}

unsigned PwmModulator::quanta(unsigned chunk, bool noFloat) {
  if (noFloat) {
    //the below ignores the fraction part of dither, in order to not use floating point
    return ::quanta(chunk, quoticks);
  } else {
    return ratio(double(chunk), quoticks + Twiddler::getRatio());
  }
}
