#include "interval.h"

Interval::Interval(double highest, double lowest ): Ranged (highest, lowest){
  //#nada
}


bool Interval::setParams(ArgSet&args){
  set(highest,args);
  set(lowest,args);
  return isModified();
}

void Interval::getParams(ArgSet&args) const {
  args.next() = highest;
  args.next() = lowest;
}
