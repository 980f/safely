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
  if(changed(lastSample,bool(input))){
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

InputEvent::InputEvent(InputMonitor &inp, double lowfilter, double highfilter):
  inp(inp){
  debounce[1]=highfilter;
  debounce[0]=lowfilter;
  lastStable=!inp;
}

void InputEvent::init(double timestamp){
  inp.init(timestamp);
  lastStable=!inp;//being picky here.
}

bool InputEvent::changed(double timestamp){
  inp.sample(timestamp);
  if(lastStable!=bool(inp)){
    if(lastStable? inp.isLow(debounce[0]) : inp.isHigh(debounce[1])){
      lastStable=!lastStable;
      return true;
    }
  }
  return false;
}
