//(C) 2017 by Andrew L. Heilveil
#include "continuedfractionratiogenerator.h"
#include "minimath.h"

/*
 element 0 holds provisional new value
 element 1 holds last computed value
 element 2 holds value before that of element 1
 In the wikipedia article these match the subscripts -1, and -2, just drop the article's minus signs.

 */

ContinuedFractionRatioGenerator::ContinuedFractionRatioGenerator() {
  //confused the pee out of debug, and the thing is not usable anyway: restart(0);
  fraction = 0.0;
}

static void shift(unsigned hk[3]) {
  hk[2] = hk[1];//prior last value becomes 2 back
  hk[1] = hk[0];//most recent computed value becomes 'prior'
  //hk[0] is now stale, we leave it untouched for debug.
}

bool ContinuedFractionRatioGenerator::bump(unsigned hk[3]) {
  auto provisional = an * hk[1] + hk[2]; //uintmax_t: using extra bits to not have to deal with overflow here.
  if (provisional <= limit) { //using <= versus just < is why the limit value is usually "all 1's" in the ValueType.
    hk[0] = provisional; //a truncation, but one we just checked to insure that it doesn't actually truncate.
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

bool ContinuedFractionRatioGenerator::restart(double ratio, unsigned limit) {
  this->limit = limit == 0 ? ~0U : limit; //there were too many defective call points, fixing it here.
  h[0]=k[0]=0;//4 debug
  if(!std::isnormal(ratio)){
    an = 0;//4 debug
    fraction = 0; //should preclude triggering obscure faults
    return false;
  }
  fraction = fabs(ratio);
  split();

  h[2]=0; h[1]=1;
  k[2]=1; k[1]=an;

  return true;
}

double ContinuedFractionRatioGenerator::approximation() const {
  return ratio(double(numerator()), double(denominator()));
}

double ContinuedFractionRatioGenerator::better() const {
  return (approximation()+ratio(double(h[2]), double(k[2])))/2;
}

void ContinuedFractionRatioGenerator::run() {
  //we use a counter to guard against bad input parameters. Termination is normally done via step() due to the 'limit' value which is default set to note overflow in the computation.
  for (unsigned steps = maxWorkingBits; steps-- > 0 && step();) {}
}

double ContinuedFractionRatioGenerator::best() {
  run();
  return approximation();
}

