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

bool CountDown::operator--(int) noexcept {
  if(counter){
    --counter;
    return true;
  } else {
    return false;
  }
}

bool CountDown::last() noexcept {
  if(counter){//test and decrement
    return done();//just became zero
  } else {
    return false;
  }
}
