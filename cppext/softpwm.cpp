#include "softpwm.h"

void SoftPWM::onDone() {
  phase ^= 1;
  restart(pair[phase] - 1);//# the polledtimer stuff adds a 1 for good luck, we don't need no stinking luck. //todo:1 guard against a zero input
  onToggle(phase);
}

void SoftPWM::trigger(){
  phase=1;
  onDone();
}

void SoftPWM::setPhase(bool highness, Ticks ticks){
  pair[highness] = ticks ?ticks: 1;
}

void SoftPWM::setCycle(Ticks low, Ticks high){
  pair[0] = low;
  pair[1] = high;
}

SoftPWM::SoftPWM(Ticks low, Ticks high, bool andStart){
  setCycle(low,high);
  if(andStart) {
    onDone();
  }
}
