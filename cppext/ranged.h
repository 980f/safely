#ifndef RANGED_H
#define RANGED_H
#include "range.h"

/** Range&gt;double&lt; was very popular, and we add some utilities for it:*/
class Ranged :public Range<double> {
public:
  Ranged();
  Ranged(double highest, double lowest);
  /** @returns the given @param raw value if it is in range, else the nearest bound. */
  double clamped(double raw) const;
  /** coerces value into range and @returns whether it was altered */
  bool clamp(double &raw) const;
  /** smallest integer such that array[value in Interval/quantum] won't be an index out of bounds*/
  u16 bin(unsigned numBins, double quantum) const;
  /** @returns the nominal value in this range associated with a fragment given by the ratio of the given arguments. The return for fragments outside of 0..1 is not specified. */
  double map(int fraction, int fullscale)const;
  /** @return value that is @param fraction of range. e.g. fraction=0 returns lowest value in range. */
  double map(double fraction)const;
  /** @returns a range @param factor times center on each side of @param center */
  static Ranged around(double center,double factor=0.1);
};

#endif // RANGED_H
