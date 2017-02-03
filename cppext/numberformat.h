#ifndef NUMBERFORMAT_H
#define NUMBERFORMAT_H

#include "countdown.h"
#include "numberpieces.h"
/**
 * parameters for formatting a number.
 * It is an object so that we can pass it to a templated printf-like function as an argument that precedes the numbers it applies to.
 * There is a feature 'usages' that causes it to reset to default values after a specified number of uses.@see onUse()
 *
 * when one of these is an arg... list we just store its attributes to another one, so copy-assign must be cheap. */
struct NumberFormat {
  /** see printf x.y rules, the x goes to fieldWidth but the sign and y go here. */
  int precision;
  /** minimum width, pad with spaces */
  unsigned fieldWidth;
  /** if true always show sign,  */
  bool showsign;
  /** how many values it is to be applied to before being forgotten. 0 is forever, ~0 is nearly forever */
  CountDown usages = 0;//default unlimited

  NumberFormat();

  /** worst case required*/
  unsigned needs() const;

  /** worst case required*/
  unsigned needs(double value,NumberPieces *preprint=nullptr) const;

  /** set to standard values */
  void clear();

  /** every time this is used to actuall process a number someone should call this method*/
  void onUse();
};

#endif // NUMBERFORMAT_H
