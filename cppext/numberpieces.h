#pragma once // "(C) Andrew L. Heilveil, 2017"

#include <minimath.h>

#include "eztypes.h"

/** a description of the human-readable form of a number. */
class NumberPieces {
protected:
  static const unsigned base = 10; //someday make this dynamic.
public:
  bool isNan;
  bool isInf;
  bool isZero;
  bool negative;

  /** whether mantissa had a decimal point*/
  bool hadRadixPoint;
  /** actual digits left of decimal point, not the count thereof */
  u64 predecimal;
  /** additional digits found when parsing. If not zero then digits after decimal have been dropped. */
  unsigned pow10;
  /** actual digits after zero, divide by 10^postDigits for their mathematical value */
  u64 postdecimal;
  /** how many post decimal digits are represented by 'postdecimal' value */
  unsigned postDigits;
  /** whether an explicit exponent was encountered */
  bool hasEterm;
  /** whether an explicit exponent was negative*/
  bool negativeExponent;
  /** true exponent */
  u64 exponent; //this large just so that we can easily share a function, 10 bits would suffice for ieee754 double.
  /** set as if we just saw zero*/
  void reset();

  /** assemble pieces into an actual fp representation */
  double packed() const;

  /** @returns whether number could be an integer.
   * @deprecated still returns true even if the result overflows the number of bits, not yet properly handling exponent.
   */
  bool seemsInteger(unsigned numBits= std::numeric_limits<unsigned>::digits) const;

  template <typename Numeric> bool seemsInteger() const {
    return seemsInteger(std::numeric_limits<Numeric>::digits);
  }

  /** saturated signed version of number predecimal */
  s64 asInteger() const;

  /** create clean one*/
  NumberPieces() {
    reset();
  }

  /** sets fields according to @param d */
  NumberPieces(double d);

  bool seemsOk() const;

  /** @returns whether the given character is allowed at the start of a number (excludes 0, we don't do 'C' binary formats here */
  static bool startsNumber(char c);

  void decompose(double d);

  /** maximum value that can be multiplied by 10 and not exceed 2^64: */
  static constexpr u64 DecimalCutoff = (1ULL << 63) / 5; //2^64 /10 == 2^63/5, needed to take care that the compiler didn't get a wrap.
  static constexpr unsigned maxDigits = 19; /* (floor(log10(2^64) */
  static constexpr double p19 = 10e19;
};
