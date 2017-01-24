#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H

#include "eztypes.h"

/** number parser (think atoi/atod/atol) receiver style */
class NumberParserPieces {
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
  int pow10;
  u64 postdecimal;
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
  NumberParserPieces(){
    reset();
  }

  bool seemsOk() const;

  /** @returns whether the given character is allowed at the start of a number (excludes 0, we don't do 'C' binary formats here */
  static bool startsNumber(char c);
}; // class NumberParserState


/** a parser that is fed characters sequentially */
class PushedNumberParser : public NumberParserPieces {
  enum Phase {
    Start, BeforeDecimal, AfterDecimal, AfterExponent, Done, Failed
  };
  Phase phase;
public:
  /** char counter, handy for caller to note where parsing stopped, ordinal of char which terminated it. */
  unsigned processed=0;
  /** value of NPS::double cached when next() returned true */
  double lastParsed;

public:
  /** prepare for new number*/
  void reset();

  /** @returns whether more chars are possbile (not terminated) */
  bool next(char u);

  /** @returns net power of 10, including pre-normalized mantissa */
  int ilog10()const;

  PushedNumberParser(){
    reset();
  }

  /** computes value from pieces*/
  double value();

private:
  /** @returns whether accum had room without overflow/wrapping */
  bool applydigit(u64 &accum, char ch);
  static bool isExponentMarker(char ch);
  bool fail();

}; // class PushedNumberParser




#endif // NUMBERPARSER_H
