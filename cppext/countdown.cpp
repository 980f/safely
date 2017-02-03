#include "countdown.h"

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
    return operator--(0);//just became zero
  } else {
    return false;
  }
}
