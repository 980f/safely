#ifndef NUMBERFORMATTER_H
#define NUMBERFORMATTER_H

#include "safely.h"
#include "sigcuser.h"
#include "textpointer.h"

/** abstract number printer, convert number into text. */
typedef sigc::slot< Text, double /*value*/ > Formatter;

struct NumberFormatter {
  /** meaning of @see precision field */
  bool fp; //else fixed decimals
  /** if fp then number of decimals, else field width right aligned. */
  int precision;//exposed for dynamic control
  /** commonly used for unit-of-measure or SI multiplier. Note: this does not automatically add a space between the digits and this text, do that yourself. */
  Text postfix;

  NumberFormatter(bool fp, int precision, TextKey postfix = "");

  /** @param addone if true adds one to use value of precision without altering the field */
  Text format(double value,bool addone = false) const;

  Text operator ()(double value,bool addone = false) const {
    return format(value,addone);
  }
};


#endif // NUMBERFORMATTER_H
