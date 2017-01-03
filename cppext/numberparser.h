#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H

#include "eztypes.h"

//#include "buffer.h" //the data type for our parser.

/** number parser (think atoi/atod/atol) that works on safe arrays (this library's bounds checked storage access) */
class NumberParserPieces {
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

  /** @returns whether the given character is allowed at the start of a number (excludes 0, we don't do 'C' binary formats here */
  static bool startsNumber(char c);
}; // class NumberParserState


/** a parser that is fed characters sequentially */
class PushedNumberParser : public NumberParserPieces {
  /** value of NPS::double cached when next() returned true */
  double lastParsed;
  unsigned processed=0;
  /** whitespace ignored */
  unsigned skipped=0;
  /** last char fed to next, retained for debug, otherwise would be a local of next() */
  char ch=0;
public:
  /** prepare for new number*/
  void reset();
  /** @returns whether the pushed char terminated number input. */
  bool next(char u);
  PushedNumberParser(){
    reset();
  }
}; // class PushedNumberParser


//struct NumberParser : public NumberParserState {
//  static int parseUnsigned(u64&n, PeekableSequence<char >&p);
//  void parseFraction(PeekableSequence<char >&p);
//  bool parseNumber(LatentSequence<char >&p);
//  /** parseNumber followed by packing it into a double, on error get NaN*/
//  double getValue(LatentSequence<char >&p);
//};

#endif // NUMBERPARSER_H
