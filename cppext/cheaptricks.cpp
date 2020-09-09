#include "cheaptricks.h"
#include "minimath.h"


bool changed(double&target, double newvalue,int bits){
  if(!nearly(target,newvalue,bits)) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

