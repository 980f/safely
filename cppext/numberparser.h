#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H

#include "eztypes.h"

#include "buffer.h"

/** number parser (think atoi/atod/atol) that works on safe arrays (this library's bounds checked storage access) */

class NumberParserState {
public:
  bool isNan;
  bool isInf;
  bool negative;
  /** whether mantissa had a decimal point*/
  bool hadRadixPoint;
  u64 predecimal;
  int pow10;
  u64 postdecimal;
  int div10;
  bool hasEterm;
  bool negativeExponent;
  u64 exponent; //this large just so that we can easily share a function
  void reset(void);
  double packed() const;
  /** @returns whether number could be an integer */
  bool seemsInteger() const;
  /** saturated signed version of number predecimal */
  s64 asInteger() const;
  NumberParserState(){
    reset();
  }

  static bool startsNumber(char c);
}; // class NumberParserState

struct NumberParser : public NumberParserState {
  static int parseUnsigned(u64&n, PeekableSequence<char >&p);
  void parseFraction(PeekableSequence<char >&p);
  bool parseNumber(LatentSequence<char >&p);
  /** parseNumber followed by packing it into a double, on error get NaN*/
  double getValue(LatentSequence<char >&p);
};

class PushedNumberParser : public NumberParserState {
  /** value of NPS::double cached when next() returned true */
  double lastParsed;
  int processed;
  /** whitespace ignored */
  int skipped;
  char ch;  //init for debug
  /** prepare for new number*/
  void reset();
  /** @returns whether the pushed char terminated number input. */
  bool next(char u);
}; // class PushedNumberParser

#endif // NUMBERPARSER_H
