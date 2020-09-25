#pragma once  //(C) 2020 Andy Heilveil (980F)

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#include "ranged.h"
#include "settable.h"

/** a floating point range that is transportable via ArgSet.*/
class Interval : public Ranged, public Settable {
public:
  Interval(double highest = Nan, double lowest = Nan);

  unsigned numParams() const override {
    return 2;
  }

  /**@returns whether settings are modified*/
  bool setParams(ConstArgSet &args) override;
  void getParams(ArgSet &args) const override;

  void operator=(const Range<double> &other) {
    Range<double>::operator=(other);
  }
};

#pragma clang diagnostic pop
