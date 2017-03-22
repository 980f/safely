#ifndef NUMBERPIECES_H
#define NUMBERPIECES_H

#include "eztypes.h"

/** originally part of parsing the member names and descriptions are related to that, although printing now uses this class. */
class NumberPieces {
protected:
  static const unsigned base=10;//someday make this dynamic.
public:
  bool isNan;
  bool isInf;
  bool isZero;
  bool negative;
  /** whether mantissa had a decimal point*/
  bool hadRadixPoint;
  u64 predecimal;
  /** additional zeros to append to predecimal. If not zero then can ignore postdecimal and div10 */
  int pow10;
  /** digits after zero, divide by 10^div10 for their mathematical value */
  u64 postdecimal;
  /** how many post decimal digits are represented by 'postdecimal' value */
  int div10;
  /** whether an explicit exponents was encountered */
  bool hasEterm;
  /** whether an explicit exponent was negative*/
  bool negativeExponent;
  /** explicit exponent (only valid if hasEterm is true) */
  u64 exponent; //this large just so that we can easily share a function
  /** set as if we just saw zero*/
  void reset(void);
  /** assemble pieces into an actual fp representation */
  double packed() const;
  /** @returns whether number could be an integer */
  bool seemsInteger() const;
  /** saturated signed version of number predecimal */
  s64 asInteger() const;

  /** create clean one*/
  NumberPieces(){
    reset();
  }

  /** parse a number */
  NumberPieces(double d);

  bool seemsOk() const;

  /** @returns whether the given character is allowed at the start of a number (excludes 0, we don't do 'C' binary formats here */
  static bool startsNumber(char c);

  void decompose(double d);

  /** maximum value that can be multiplied by 10 and not exceed 2^64: */
  const u64 DecimalCutoff = (1ULL<<63)/5; //2^64 /10 == 2^63/5, needed to take care that the compiler didn't get a wrap.

}; // class NumberParserState



#endif // NUMBERPIECES_H
