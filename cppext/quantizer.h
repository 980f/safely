#ifndef Quantizer_h
#define Quantizer_h
#include "range.h"

/**
  * first use is converting a range of digital values
  * to floating point,
  * with bounds checking.
  */
class Quantizer {
public:
  //and since these are public we can't cache anything.
  IndexRange discrete;
  Interval natural;
public:
  Quantizer& operator = (const Quantizer &other) {
    discrete = other.discrete;
    natural = other.natural;
    return *this;
  }

  int bounds(double d) const {
    return natural.cmp(d);
  }

  int bounds(u16 d) const {
    return discrete.cmp(d);
  }

  /** clamp to range */
  u16 quantize(double nat) const {
    return discrete.start() + discrete.span() * natural.fragment(natural.clamped( nat));
  }

  double interpret(u16 digital) const {
    return natural.clamped(raw(digital));
  }

  double raw(u16 digital) const {
    //even if out of bound convert it:
    return natural.start() + natural.width() * discrete.fragment(digital);
  }

};

#endif // LINEARTRANSFER_H
