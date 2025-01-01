//(C) 2017 by Andrew L. Heilveil
#include "continuedfractionratiogenerator.h"
#include "minimath.h"


ContinuedFractionRatioGenerator::ContinuedFractionRatioGenerator() {
  restart(0);
}

bool ContinuedFractionRatioGenerator::restart(double ratio, unsigned limit) {
  this->limit = limit == 0 ? ~0U : limit; //there were too many defective call points, fixing it here.
  fraction = fabs(ratio);
  h[0] = k[1] = 0; //note that indices here are swap of following ones. h/k 0/1 is a 2x2 matrix that we are initializing to the identity matrix.
  h[1] = k[0] = 1;
  an = h[2] = k[2] = 0; //4 debug, these values aren't actually used.
  step(); //gives 1/0
  return step(); //gives int(ratio)/1
}

bool ContinuedFractionRatioGenerator::bump(unsigned hk[3]) {
  uintmax_t provisional = uintmax_t(an) * hk[1] + hk[0]; //uintmax_t: using extra bits to make the math easier here.
  if (provisional <= limit) {
    hk[2] = unsigned(provisional);
    return true;
  }
  return false;
}

static void shift(unsigned hk[3]) {
  hk[0] = hk[1];
  hk[1] = hk[2];
}

bool ContinuedFractionRatioGenerator::step() {
  if (split()) {
    if (bump(h) && bump(k)) { //only  if both are in range do we keep the new values
      shift(h);
      shift(k);
      return true;
    }
    return false; //breakpoint here to stop at final step
  }
  return false;
}

double ContinuedFractionRatioGenerator::approximation() {
  return ratio(double(numerator()), double(denominator()));
}

void ContinuedFractionRatioGenerator::run() {
  //we use a counter to guard against bad input parameters. Termination is normally done via step() due to the 'limit' value which is default set to note overflow in the computation.
  for (unsigned steps = maxWorkingBits; steps-- > 0 && step();) {}
}

double ContinuedFractionRatioGenerator::best() {
  run();
  return approximation();
}

bool ContinuedFractionRatioGenerator::split() {
  static const double uint32_tepsilon = pow(2, -maxWorkingBits); //confirmed perfect representation. 0x3df0000000000000
  if (fraction == 0.0) {
    return false;
  }

  if (fraction < uint32_tepsilon) { //subsequent math would overflow without notices.
    return false;
  }

  double inverse = 1.0 / fraction;

  an = splitteru(inverse);
  fraction = inverse;
  return true;
}
