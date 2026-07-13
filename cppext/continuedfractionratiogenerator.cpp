//(C) 2017 by Andrew L. Heilveil
#include "continuedfractionratiogenerator.h"
#include "minimath.h"

const double ContinuedFractionRatioGenerator::epsilon = std::pow(2.0, -32);//ContinuedFractionRatioGenerator::maxWorkingBits); //confirmed perfect representation. 0x3df0000000000000


ContinuedFractionRatioGenerator::ContinuedFractionRatioGenerator() {
  //confused the pee out of debug, and the thing is not usable anyway restart(0);
  fraction = 0.0;
}

static void shift(unsigned hk[3]) {
  hk[1] = hk[0];
  hk[2] = hk[1];
}

bool ContinuedFractionRatioGenerator::restart(double ratio, unsigned limit) {
  this->limit = limit == 0 ? ~0U : limit; //there were too many defective call points, fixing it here.
  if(!std::isnormal(ratio)){
    fraction = 0; //should preclude triggering obscure faults
    return false;
  }
  fraction = 1.0 / fabs(ratio);
  h[0] = k[1] = 0; //note that indices here are swap of following ones. h/k 0/1 is a 2x2 matrix that we are initializing to the identity matrix.
  h[1] = k[0] = 1;
  //the following should not actually get used, but prevent getting distracted during debug:
  h[2] = h[1];
  k[2] = k[1];

  split(); //gives 1/0
  shift(h);
  h[0]=an;
  shift(k);
  k[0]=1;
  return step(); //gives int(ratio)/1
}

bool ContinuedFractionRatioGenerator::bump(unsigned hk[3]) {
  uintmax_t provisional = uintmax_t(an) * hk[1] + hk[2]; //uintmax_t: using extra bits to make the math easier here.
  if (provisional <= limit) {
    hk[0] = unsigned(provisional);
    return true;
  }
  return false;
}



bool ContinuedFractionRatioGenerator::step() {
  if (split()) {
    if (bump(h) && bump(k)) { //only if both are in range do we keep the new values
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
  if (fraction == 0.0) {
    return false;
  }

  if (fraction < epsilon) { //subsequent math would overflow without notices.
    return false;
  }

  double inverse = 1.0 / fraction;

  an = splitteru(inverse);
  fraction = inverse;
  return true;
}
