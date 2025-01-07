//"(C) Andrew L. Heilveil, 2017"
#include "pushedparser.h"

#include "cstr.h"
#include "utf8.h"
#include "onexit.h"

void PushedParser::reset(bool fully) {
  span.clear();
  // phase = Before;
  wasQuote = endQuote = 0;
  if (fully) {
    d.reset();
  }
}

void PushedParser::shift(unsigned offset) {
  span.shift(offset); //this is the line that matters most
  d.shift(offset);
}

PushedParser::PushedParser() {
  reset(true);
}

void PushedParser::Diag::apply(char pushed) {
  last = pushed;
  if (pushed == '\n') {
    column = 1; //match pre-increment below.
    ++row;
  } else {
    ++column;
  }
}

void PushedParser::Diag::shift(unsigned offset) {
  //only correct for normal use cases, where shift is within the same text line. Might get fancier once we have test cases with which to decide what should really be done.
  location -= offset;
  if (offset < column) {
    column -= offset;
  } else {
    column = 0;//someone who shifts more than a line's worth is on their own.
  }
}

/** @param andItem should be true when the seperator marks a prefix vs the end of a value.
 Basically it indicates a string terminated by a token rather than whitespace, whitespace is not a separator.
 @returns Done, but someday we might find something illegal to complain about. */
PushedParser::Action PushedParser::endValue() {
  if (span.lowest.isValid()) {
    if (span.highest.isValid()) {
      //then field is already closed
    } else {
      span.highest = d.location;
    }
  }
  return Done;
}

// void PushedParser::beginValue() {
//   // phase = Inside;
//   span.highest = BadIndex; //for safety
//   span.lowest = d.location;
// }

/* priorities:
 * check for null
 * check for endquote
 * check for separator
 * check for startquote
 * only now check for white space
 */
PushedParser::Action PushedParser::next(char pushed) {
  IncrementOnExit<unsigned> ioe(d.location); //increment before we dig deep lest we forget given the multiple returns.
  d.apply(pushed);
  Char ch(pushed);

  if (pushed == 0) { //end of stream/line
    if (endQuote) { //waiting for an end quote we found and end of stream
      return Illegal;
    } else {
      if (rule.nullIsSeperator) {
        return endValue();
      }
      return Illegal;
    }
  }

  if (endQuote) {
    if (ch.is(endQuote)) {
      endQuote = 0;
      //provisional end of field
      span.include(d.location); //starts OR stretches
      //but we are still 'Inside' so that adjacent quote pieces are still just one field.
      return Continue;
    }
    return Continue; // quoting hides everything except endquote
  }

  if (ch.in(rule.seperators)) {
    if (span.highest == BadIndex) { //only make this the end if no endquote or whitespace has been encountered
      span.highest = d.location;
    }
    return Done;
  }

  //checking for start of quote
  Cstr pairings(rule.quoters);
  auto which = pairings.index(ch);
  if (which != BadIndex && !which & 1) { //then we have a start quote
    wasQuote = ch;
    endQuote = rule.quoters[which + 1];
    // phase = Inside;
    //we do NOT start the span here, the start quote is not part of the token
    return Continue;
  }

  //so to get here the char is not end of input, endQuote, a separator, a start quote
  if (!ch.in(rule.trimmers)) {
    span.include(d.location);
    //to us it does not exist :)
  }
  //must be whitespace, and we don't react to that regardless of where we are in tokenization.
  return Continue;
}
