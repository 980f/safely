#pragma once // "(C) Andrew L. Heilveil, 2017-2018"

#include "countdown.h"
#include "numberpieces.h"
/**
 * parameters for formatting a number.
 * It is an object so that we can pass it to a templated printf-like function as an argument that precedes the numbers it applies to.
 * There is a feature 'usages' that causes it to reset to default values after a specified number of uses.@see onUse()
 *
 * when one of these is an arg... list we just store its attributes to another one, so copy-assign must be cheap. */

struct NumberFormat {
  /** number of digits after radix, if negative then add zeroes before the radix point */
  int decimals;
  /** minimum width, pad with spaces */
  unsigned fieldWidth;
  /** if true always show sign, else no room is taken for positives */
  bool showsign;
  /** if true then decimals is sigfigs */
  bool scientific;
  /** how many values it is to be applied to before being forgotten. 0 is forever, ~0 is nearly forever */
  CountDown usages = 0;//default unlimited

  NumberFormat();

  unsigned minCharsNeeded() const;

  /** worst case required*/
  unsigned needs() const;

  /** @returns fair estimate of what is required*/
  unsigned needs(double value,NumberPieces *preprint=nullptr) const;

  /** set to standard values */
  void clear();

  /** every time this is used to actually process a number someone should call this method.
   * when the designated number of usages has been reached this object is altered to the default (clear()) state. */
  void onUse();
};
