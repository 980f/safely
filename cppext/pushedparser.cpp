//"(C) Andrew L. Heilveil, 2017"
#include "pushedparser.h"

#include "utf8.h"
#include "onexit.h"

void PushedParser::itemCompleted() {
  value.clear();
  phase = Before;
  inQuote = false; //usually already is, except on reset
}

void PushedParser::reset(bool fully) {
  itemCompleted();
  if (fully) {
    d.reset();
  }
}

void PushedParser::shift(unsigned offset) {
  value.shift(offset); //this is the line that matters most
  d.shift(offset);
}

PushedParser::PushedParser() {
  reset(true);
}

void PushedParser::Diag::shift(unsigned offset) {
  //only correct for normal use cases, where shift is within the same text line. Might get fancier once we have test cases with which to decide what should really be done.
  location -= offset;
  column = location;
}

const char *PushedParser::lookFor(const char *seps) {
  return postAssign(seperators, seps);
}

/** @param andItem should be true when the seperator marks a prefix vs the end of a value.
 Basically it indicates a string terminated by a token rather than whitespace, whitespace is not a separator.*/
PushedParser::Action PushedParser::endValue(bool andItem) {
  wasQuoted = take(inQuote);
  value.highest = d.location;
  phase = andItem ? Before : After;
  return andItem ? EndValueAndItem : EndValue;
}

void PushedParser::beginValue() {
  phase = Inside;
  value.highest = BadIndex; //for safety
  value.lowest = d.location;
}

PushedParser::Action PushedParser::next(char pushed) {
  IncrementOnExit<unsigned> ioe(d.location); //increment before we dig deep lest we forget given the multiple returns.
  d.last = pushed;

  if (pushed == '\n') {
    d.column = 1; //match pre-increment below.
    ++d.row;
  } else {
    ++d.column;
  }

  UTF8 ch(pushed);

  if (pushed == 0) { //end of stream
    if (inQuote) {
      switch (phase) {
        case Before: //file ended with a start quote.
          return Illegal;
        case Inside: //act like endquote was missing
          return endValue(true); //attempt recovery at unexpected EOF
        case After:
          return Illegal; //shouldn't be able to get here.
      }
    } else {
      switch (phase) {
        case Before:
          return Done;
        case Inside:
          return endValue(true); //attempt recovery at unexpected EOF
        case After:
          return EndItem; //eof push out any partial node
      }
    }
    return Illegal; //can't get here
  }

  if (utfFollowers) { //it is to be treated as a generic text char
    ch = 'k';//this couldh ave been any char that we are certain is not in the framing set.
  } else if (ch.isMultibyte()) { //first byte of a utf8 multibyte character
    utfFollowers = ch.numFollowers();
    ch = 'k';
  }

  //we still process it
  if (inQuote) {
    if (ch.is('"')) { //end quote
      return endValue(false); //end of a token, not sure if it is a terminal token.
    }
    if (ch.is('\\')) {
      ++utfFollowers; //ignores all escapes, especially \"
    }
    //still inside quotes
    if (phase == Before) { //first char after quote is first care of token
      beginValue();
      return BeginValue;
    }
    return Continue;
  }

  switch (phase) {
    case Inside:
      if (ch.isWhite()) { //this stanza insist that tokens not have any internal white space, use quotes when that is a problem.
        return endValue(false); //typical case: whitespace separator, separates tokens but doesn't identify what type of token.
      }
      if (ch.in(seperators)) {
        return endValue(true); //typical case: there is no whitespace between token and special character
      }
      return Continue;
    case After:
      if (ch.isWhite()) {
        return Continue;
      }
      if (ch.in(seperators)) {
        phase = Before;
        return EndItem;
      }
      return Illegal;

    case Before: //expecting name or value
      if (ch.isWhite()) {
        return Continue;
      }
      if (ch.in(seperators)) {
        endValue(true);
        return EndValueAndItem; //a null one
      }
      if (ch.is('"')) {
        inQuote = true;
        return Continue; //but not yet started in chunk, see if(inQuote)
      }
      beginValue();
      return BeginValue;
  } // switch
  return Illegal; // in case we have a missing case above, should never get here.
}
