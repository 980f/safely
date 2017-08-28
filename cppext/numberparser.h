#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H "(C) Andrew L. Heilveil, 2017"

#include "eztypes.h"

/** number parser (think atoi/atod/atol) receiver style */
#include "numberpieces.h"

/** a parser that is fed characters sequentially */
class PushedNumberParser : public NumberPieces {
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
