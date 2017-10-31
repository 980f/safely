#include "inputmonitor.h"

InputMonitor::InputMonitor(Din &input):input(input){
  lastSample=0;//the input usually isn't configured yet :(
  changes[1]=changes[0]=0;
  events[1]=events[0]=lastChecked=0;
}

void InputMonitor::init(double timestamp){
  events[1]=events[0]=lastChecked=timestamp;
  lastSample=input;
}

bool InputMonitor::sample(double timestamp){
  lastChecked=timestamp;
  if(changed(lastSample,input.operator bool())){
    ++changes[lastSample];
    events[lastSample]=lastChecked;
    return true;
  }
  return false;
}

bool InputMonitor::isHigh(double debounced) const noexcept{
  return lastSample&& ((events[1]-events[0])>=debounced);
}

bool InputMonitor::isLow(double debounced) const noexcept{
  return !lastSample && ((events[0]-events[1])>=debounced);
}

//bool InputEvent::check(double timestamp){
//  inp.sample(timestamp);
//  switch (level) {
//  case 1: return inp.isHigh(debounce);
//  case -1: return inp.isLow(debounce);
//  case 0: return fabs(inp.events[1]-inp.events[0])>debounce;

//  }
//}
