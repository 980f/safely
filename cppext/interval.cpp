#include "interval.h"

Interval::Interval(float highest, float lowest) : Rangef(highest, lowest) {
  //#nada
}

bool Interval::setParams(ConstArgSet &args) {
  set(highest, args);
  set(lowest, args);
  return isModified();
}

void Interval::getParams(ArgSet &args) const {
  args.next() = highest;
  args.next() = lowest;
}
