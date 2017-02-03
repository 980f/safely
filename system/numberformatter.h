#ifndef NUMBERFORMATTER_H
#define NUMBERFORMATTER_H

//#include "safely.h"
#include "textpointer.h"
//#include "cheaptricks.h"
#include "numberformat.h"

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
  Text operator ()(double value,bool addone = false) const;


public: //some utilities
  static Text makeNumber(double value);
};


#endif // NUMBERFORMATTER_H
