#pragma once  //(C) 2020 Andy Heilveil (980F)

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#include "rangef.h"
#include "settable.h"

/** a floating point range that is transportable via ArgSet.*/
class Interval : public Rangef, public Settable {
public:
  Interval(float highest = Nan, float lowest = Nan);

  unsigned numParams() const override {
    return 2;
  }

  /**@returns whether settings are modified*/
  bool setParams(ConstArgSet &args) override;
  void getParams(ArgSet &args) const override;

  void operator=(const Range<float> &other) {
    Range<float>::operator=(other);
  }
};

#pragma clang diagnostic pop
