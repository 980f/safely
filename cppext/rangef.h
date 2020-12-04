#pragma once
#include "range.h"

/** Range&gt;double&lt; was very popular but we now have hardware float but not hardware double so we cloned and edited the class. */
class Rangef :public Range<float> {
public:
  Rangef();
  constexpr Rangef(float highest, float lowest):Range(highest,lowest){
    //#nada
  }

  /** @returns the given @param raw value if it is in range, else the nearest bound. */
  float clamped(float raw) const;
  /** coerces value into range and @returns whether it was altered */
  bool clamp(float &raw) const;
  /** smallest integer such that array[value in Interval/quantum] won't be an index out of bounds*/
  u16 bin(unsigned numBins, float quantum) const;
  /** @returns the nominal value in this range associated with a fragment given by the ratio of the given arguments. The return for fragments outside of 0..1 is not specified. */
  float map(int fraction, int fullscale)const;
  /** @return value that is @param fraction of range. e.g. fraction=0 returns lowest value in range. */
  float map(float fraction)const;
  /** @returns a range @param factor times center on each side of @param center */
  static Rangef around(float center,float factor=0.1);
};
