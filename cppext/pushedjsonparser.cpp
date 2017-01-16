#include "safely.h"  //JOIN
#include "pushedjsonparser.h"
#include "utf8.h"
#include "localonexit.h"

using namespace PushedJSON;

//Action Lexer::next(char pushed){
//  UTF8 ch(pushed);

//  if(pushed==0){//end of stream
//    if(inQuote) {
//      switch (phase) {
//      case Before: //file ended with a start quote.
//        return Illegal;
//      case Inside: //act like endquote was missing
//        return EndItemT;
//      case After:
//        return Illegal;//shouldn't be able to get here.
//      }
//    } else {
//      switch (phase) {
//      case Before:
//        return EndItemT;
//      case Inside:
//        return EndItemT;
//      case After:
//        return EndItem; //eof push out any partial node
//      }
//    }
//    return Illegal;//can't get here
//  }

//  if(utfFollowers) {//it is to be treated as a generic text char
//    ch = 'k';
//  } else if(ch.isMultibyte()) {//first byte of a utf8 multibyte character
//    utfFollowers = ch.numFollowers();
//    ch = 'k';
//  }

//  //we still process it
//  if(inQuote) {
//    if(ch.is('"')) {//end quote
//      inQuote = false;
//      phase = After;
//      return EndQuoted;
//    }
//    if(ch.is('\\')){
//      ++utfFollowers;//ignores all escapes, especially \"
//    }
//    //still inside quotes
//    if(phase==Before) {//first char after quote is first care of token
//      phase = Inside;
//      return BeginToken;
//    }
//    return Continue;
//  }

//  switch (phase) {
//  case Inside:
//    if(ch.isWhite()) {
//      phase = After;
//      return EndToken;
//    }
//    switch(ch) {
//    case ':': //normal name value seperator
//      //we have ended a token and the name,
//      phase = Before;
//      return EndNameT;
//    case '"': //not ours to reason why
//      return Illegal;
//    case '{': //?missing colon
//      return Illegal;
//    case '}': //no whitespace end.
//      phase = Before;
//      return EndWadT;
//    case ',': //normal item seperator
//      phase = Before;
//      return EndItemT;
//    default:
//      return Continue;
//    } // switch
//    break;

//  case After:
//    if(ch.isWhite()) {
//      return Continue;
//    }
//    switch(ch) {
//    case ':': //normal name value seperator
//      phase = Before;
//      return EndName;
//    case '"':
//      return Illegal; //maydo: act like missing coma
//    case '{':
//      return BeginWad;
//    case '}'://after last value of a wad
//      return EndWad;
//    case ',': //normal item seperator
//      return EndItem; //normal child endwith trailing whitespace
//    default:
//      return Illegal;
////      phase = Inside;
////      return BeginToken;
//    } // switch
//    break;

//  case Before:  //expecting name or value
//    if(ch.isWhite()) {
//      return Continue;
//    }
//  } // switch
//  return Illegal;// in case we have a missing case above, should never get here.
//} // Lexer::next


void Parser::recordName(){
  name.take(value);
  quotedName=postAssign(wasQuoted,false);
  haveName = true;
}

PushedJSON::Action Parser::next(char pushed){

  switch (PushedParser::next(pushed)) {
  case BeginValue:
  case EndValue:
  case Continue:
    return PushedJSON::Continue;

  case Illegal:
    return PushedJSON::Illegal;
  case Done:
    return PushedJSON::Done;

  case PushedParser::EndValueAndItem:
  case PushedParser::EndItem:
    switch(d.last) {
    case ':':
      recordName();
      return PushedJSON::Continue; //null name is not the same as no name
    case '{': //normal
      return BeginWad;
    case '}': //normal
      return EndWad;
    case ',': //sometimes is an extraneous comma, we choose to ignore those.
      return PushedJSON::EndItem;//missing value, possible missing whole child.
    case 0: /*abnormal*/
      return PushedJSON::EndItem;//ok for single value files, not so much for normal ones.
    default:
      return PushedJSON::Illegal;
    } // switch
  }
  return PushedJSON::Illegal;//catch regressions
}

void Parser::itemCompleted(){
  haveName = false;
  quotedName=false;
  name.clear();
  value.clear();
  PushedParser::itemCompleted();
} // Parser::next

void Parser::reset(bool fully){
  PushedParser::reset(fully);
  itemCompleted();
}

void Parser::shift(unsigned offset){
  if(haveName){//gratuitous test, name can shift for itself even when not valid.
    name.shift(offset);
  }
  PushedParser::shift(offset);
}

Parser::Parser(){
  lookFor(":{,}");
  reset(true);
}



JsonStats::DepthTracker::DepthTracker(JsonStats &s):CountedLock(s.nested){
  s.maxDepth.inspect(s.nested);//heuristics
}

void JsonStats::reset(){
  totalNodes = 0;
  totalScalar = 0;
  maxDepth.reset();
  nested = 0;
}

void JsonStats::onNode(bool scalar){
  ++totalNodes;
  if(scalar){
    ++totalScalar;
  }
}
