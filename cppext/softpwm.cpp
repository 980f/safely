#include "softpwm.h"

SoftPWM::SoftPWM(Ticks low, Ticks high, bool andStart){
  setCycle(low,high);
  if(andStart) {
    onDone();
  }
}
