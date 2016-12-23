#ifndef NUMBERFORMATTER_H
#define NUMBERFORMATTER_H

#include "safely.h"
#include "textpointer.h"

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
  unsigned usages = 0;//default unlimited

  NumberFormat();

  /** worst case required*/
  unsigned needs() const;
  /** set to standard values */
  void clear();

  /** every time this is used to actuall process a number someone should call this method*/
  void onUse();
};


struct NumberFormatter {
  /** meaning of @see precision field */
  NumberFormat nf;
  /** commonly used for unit-of-measure or SI multiplier. Note: this does not automatically add a space between the digits and this text, do that yourself. */
  Text postfix;

  NumberFormatter(int precision, TextKey postfix = "");

  unsigned needs() const;

  /** @returns a locally created buffer that holds the image of @param value.
   * @param addone if true adds one to use value of precision without altering the field */
  Text format(double value,bool addone = false) const;

  /** syntactic sugar for the only method call that is usually used dynamically */
  Text operator ()(double value,bool addone = false) const {
    return format(value,addone);
  }

};


#endif // NUMBERFORMATTER_H
