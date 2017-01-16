#include "pushedparser.h"

#include "utf8.h"
#include "localonexit.h"

void PushedParser::itemCompleted(){
  value.clear();
  phase=Before;
  inQuote=false;//usually already is, except on reset
} // Parser::next

void PushedParser::reset(bool fully){
  itemCompleted();

  if(fully) {
    d.location = 0;
    d.row = 0;
    d.column = 0;
    d.last=0;
  }
}

void PushedParser::shift(unsigned offset){
  d.column=d.location-offset;//only correct for normal use cases
  value.shift(offset);
  d.location-=offset;
}

PushedParser::PushedParser(){
  reset(true);
}

const char *PushedParser::lookFor(const char *seps){
  return postAssign(seperators,seps);
}

PushedParser::Action PushedParser::endValue(bool andItem){
  wasQuoted=postAssign(inQuote,false);
  phase = andItem?Before:After;
  value.highest=d.location;
  return andItem?EndValueAndItem:EndValue;
}

void PushedParser::beginValue(){
  phase = Inside;
  value.highest = BadIndex; //for safety
  value.lowest = d.location;
}

PushedParser::Action PushedParser::next(char pushed){
  IncrementOnExit<unsigned> ioe(d.location);//increment before we dig deep lest we forget given the multiple returns.
  d.last=pushed;

  if(pushed=='\n') {
    d.column = 1;//match pre-increment below.
    ++d.row;
  } else {
    ++d.column;
  }

  UTF8 ch(pushed);

  if(pushed==0){//end of stream
    if(inQuote) {
      switch (phase) {
      case Before: //file ended with a start quote.
        return Illegal;
      case Inside: //act like endquote was missing
        return endValue(true);
      case After:
        return Illegal;//shouldn't be able to get here.
      }
    } else {
      switch (phase) {
      case Before:
        return Done;
      case Inside:
        return endValue(true);
      case After:
        return EndItem; //eof push out any partial node
      }
    }
    return Illegal;//can't get here
  }

  if(utfFollowers) {//it is to be treated as a generic text char
    ch = 'k';
  } else if(ch.isMultibyte()) {//first byte of a utf8 multibyte character
    utfFollowers = ch.numFollowers();
    ch = 'k';
  }

  //we still process it
  if(inQuote) {
    if(ch.is('"')) {//end quote
      return endValue(false);
    }
    if(ch.is('\\')){
      ++utfFollowers;//ignores all escapes, especially \"
    }
    //still inside quotes
    if(phase==Before) {//first char after quote is first care of token
      beginValue();
      return BeginValue;
    }
    return Continue;
  }

  switch (phase) {
  case Inside:
    if(ch.isWhite()) {
      return endValue(false);
    }
    if(ch.in(seperators)){
      return endValue(true);
    }
    return Continue;
//    break;//end Inside

  case After:
    if(ch.isWhite()) {
      return Continue;
    }
    if(ch.in(seperators)){
      phase=Before;
      return EndItem;
    }
    return Illegal;
//    break;

  case Before:  //expecting name or value
    if(ch.isWhite()) {
      return Continue;
    }
    if(ch.in(seperators)){
      endValue(true);
      return EndValueAndItem;//a null one
    }
    if(ch.is('"')){
      inQuote = true;
      return Continue;//but not yet started in chunk, see if(inQuote)
    }
    beginValue();
    return BeginValue;
  } // switch
  return Illegal;// in case we have a missing case above, should never get here.
}
