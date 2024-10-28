//(C) 2017 by Andrew L. Heilveil
#include "continuedfractionratiogenerator.h"
#include "minimath.h"

ContinuedFractionRatioGenerator::ContinuedFractionRatioGenerator(){
  restart(0);
}

bool ContinuedFractionRatioGenerator::restart(double ratio, unsigned limit){
  this->limit = limit==0 ? ~0U : limit;//too many defective call points, fix it here.
  fraction = fabs(ratio);
  h[0] = k[1] = 0;
  h[1] = k[0] = 1;
  h[2] = k[2] = 0;//4 debug
  step();//gives 1/0
  return step();//gives int(ratio)/1
} // ContinuedFractionRatioGenerator::restart


bool ContinuedFractionRatioGenerator::bump(unsigned hk[3]){
  u64 provisional = u64(an) * hk[1] + hk[0];
  if(provisional<=limit) {
    hk[2] = unsigned(provisional);
    return true;
  }
  return false;
}

static void shift(unsigned hk[3]){
  hk[0] = hk[1];
  hk[1] = hk[2];
}

bool ContinuedFractionRatioGenerator::step(){
  if(split()) {
    if(bump(h)&& bump(k)) {
      shift(h);
      shift(k);
      return true;
    } else {
      return false;
    }
  }
  return false;
} // ContinuedFractionRatioGenerator::step

double ContinuedFractionRatioGenerator::approximation(){
  return ratio(double(numerator()),double(denominator()));
}

double ContinuedFractionRatioGenerator::best(){
  //we use a counter to guard against bad input parameters. Termination is normally done due to the 'limit' value which is default set to note overflow in the computation.
  for (unsigned steps = 32; steps-->0&&step();) {
  }
  return approximation();
} // ContinuedFractionRatioGenerator::step

bool ContinuedFractionRatioGenerator::split(){
  static const double u32epsilon = pow(2,-32);//confirmed perfect representation. 0x3df0000000000000
  if(fraction==0.0) {
    return false;
  }

  if(fraction< u32epsilon) {//subsequent math would overflow without notices.
    return false;
  }

  double inverse = 1.0 / fraction;

  an = splitteru(inverse);
  fraction = inverse;
  return true;
} // ContinuedFractionRatioGenerator::split
