#ifndef INTERVAL_H
#define INTERVAL_H

#include "ranged.h"
#include "settable.h"
/** a floating point range that is transportable via ArgSet.*/
class Interval : public Ranged,public Settable {
public:
  Interval(double highest = Nan, double lowest = Nan);

  unsigned numParams()const override {
    return 2;
  }

  /**@returns whether settings are modified*/
  bool setParams(ArgSet&args)override ;
  void getParams(ArgSet&args) const override ;

  void operator =(const Range<double> &other){
    Range<double>::operator =(other);
  }
};

#endif // INTERVAL_H
