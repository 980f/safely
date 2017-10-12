#pragma once // "(C) Andrew L. Heilveil, 2017"

#include "textpointer.h"
#include "numberformat.h"

/** print number into string. */
struct NumberTextifier {
  /** meaning of @see precision field */
  Safely::NumberFormat nf;
  /** commonly used for unit-of-measure or SI multiplier. Note: this does not automatically add a space between the digits and this text, do that yourself. */
  Text postfix;

  NumberTextifier(int precision, TextKey postfix = nullptr);

  unsigned needs(double value) const;

  /** @returns a locally created buffer that holds the image of @param value.
   * @param addone if true adds one to use value of precision without altering the field */
  Text format(double value,bool addone = false) const;

  /** syntactic sugar for the only method call that is usually used dynamically */
  Text operator ()(double value,bool addone = false) const;


public: //some utilities
  static Text makeNumber(double value);
};

