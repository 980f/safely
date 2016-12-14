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
  u64 predecimal;
  int pow10;
  u64 postdecimal;
  int div10;
  bool hasEterm;
  bool negativeExponent;
  u64 exponent; //this large just so that we can easily share a function
  void reset(void);
  double packed()const;
  NumberParserState(){
    reset();
  }
  static bool startsNumber(char c);
};

struct NumberParser:public NumberParserState {

  static int parseUnsigned(u64&n, PeekableSequence <char >&p);
  void parseFraction(PeekableSequence <char >&p);
  bool parseNumber(LatentSequence <char >&p);
  /** parseNumber followed by packing it into a double, on error get NaN*/
  double getValue(LatentSequence <char >&p);

};

class PushedNumberParser:public NumberParserState {
  double lastParsed;
  int processed ;
  int skipped ;
  u64 acc ; //need to accept 53 bits worth for mantissa of double.
  char ch ; //init for debug


  /** prepare for new number*/
  void reset();
  /** @returns whether the pushed char terminated number input. */
  bool next(char u);
};

#endif // NUMBERPARSER_H
