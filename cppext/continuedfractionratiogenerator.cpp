//(C) 2017 by Andrew L. Heilveil
#include "continuedfractionratiogenerator.h"
#include "minimath.h"

ContinuedFractionRatioGenerator::ContinuedFractionRatioGenerator(){
  restart(0);
}

bool ContinuedFractionRatioGenerator::restart(double ratio, unsigned limit){
  this->limit=limit;
  fraction=ratio;
  h[0]=k[1]=0;
  h[1]=k[0]=1;
  return true;//todo:0 check range of args against range of unsigned
}

bool ContinuedFractionRatioGenerator::step(){
  if(split()){
    h[2]=an*h[1] + h[0];
    k[2]=an*k[1] + k[0];
    if(k[2]>limit || h[2]>limit){
      return false;//we've gone one step beyond the limit
    }
    h[0]=h[1];
    h[1]=h[2];
    k[0]=k[1];
    k[1]=k[2];
    return true;
  }
  return false;
}

bool ContinuedFractionRatioGenerator::split(){
  if(fraction==0.0){
    return false;
  }
  double inverse=1.0/fraction;
  if(isNan(inverse)){
    return false;
  }

  int nextterm=splitter(inverse);//more bounds checks needed
  fraction = inverse;
  an=unsigned(nextterm);
  return true;
}
