#include <string.h>
#include "minimath.h"
#include "polynomial.h"

double polysum(double x,const double *a,int degree, unsigned prime){
  double acc = a[degree];
  if(prime==0){//expedite most used case
    for(unsigned i = degree; i-->0; ) {
      acc *= x;
      acc += a[i];
    }
  } else {
    acc*=Cnr(degree,prime);
    for(unsigned i = degree; i-- > prime; ) {
      acc *= x;
      acc += a[i] * Cnr(i,prime);
    }
  }
  return acc;
}

unsigned effectiveDegree(const double *a,unsigned degree){
  for(unsigned eff=degree;eff>=0;--eff){
    if(a[eff]!=0){
      return eff;
    }
  }
  return ~0;//indicates poly will always return 0;
}

