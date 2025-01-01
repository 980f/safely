#pragma once
#include "range.h"
#include "ranged.h"
/**
  * first use is converting a range of digital values to floating point, with bounds checking.
  */
class Quantizer {
public:
  //and since these are public we can't cache anything.
  IndexRange discrete;
  Ranged natural;
public:
  Quantizer& operator = (const Quantizer &other) {
    discrete = other.discrete;
    natural = other.natural;
    return *this;
  }

  int bounds(double d) const {
    return natural.cmp(d);
  }

  int bounds(uint16_t d) const {
    return discrete.cmp(d);
  }

  /** clamp to range */
  uint16_t quantize(double nat) const {
    return discrete.start() + uint16_t(discrete.span() * natural.fragment(natural.clamped(nat)));
  }

  double interpret(uint16_t digital) const {
    return natural.clamped(raw(digital));
  }

  double raw(uint16_t digital) const {
    //even if out of bound convert it:
    return natural.start() + natural.width() * discrete.fragment(digital);
  }

};
