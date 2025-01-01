#pragma once  // "(C) Andrew L. Heilveil, 2017"

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
  unsigned processed = 0;
  /** value of double cached when next() returned true */
  double lastParsed;

  PushedNumberParser() {
    reset();
  }

  /** prepare for new number*/
  void reset();

  /** processes @param u, @returns whether more chars are possible (parse not terminated) */
  bool next(char u);

  /** @returns net power of 10, including pre-normalized mantissa */
  int ilog10() const;


  /** computes value from pieces*/
  double value();


  /** @returns whether accum had room without overflow/wrapping.
   * an internal function that might be useful elsewhere. */
  static bool applydigit(uint64_t &accum, char ch);

private:

  static bool isExponentMarker(char ch);
  /** marks effort as failed, @returns false regardless of state, so that it can be used to "mark failed while returning false".*/
  bool fail();
}; // class PushedNumberParser
