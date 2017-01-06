#include "safely.h"  //JOIN
#include "pushedjsonparser.h"
#include "utf8.h"
#include "cheaptricks.h"

using namespace PushedJSON;

Action Lexer::next(char pushed){
  UTF8 ch(pushed);

  if(pushed==0){//end of stream
    if(inQuote) {
      switch (phase) {
      case Before: //file ended with a start quote.
        return Illegal;
      case Inside: //act like endquote was missing
        return EndItemT;
      case After:
        return Illegal;//shouldn't be able to get here.
      }
    } else {
      switch (phase) {
      case Before:
        return EndItemT;
      case Inside:
        return EndItemT;
      case After:
        return EndItem;
      }
    }
    return Illegal;//can't get here
  }

  if(utfFollowers) {//it is to be treated as a generic text char
    --utfFollowers;
    ch = 'k';
  } else if(ch.isMultibyte()) {//first byte of a utf8 multibyte character
    utfFollowers = ch.numFollowers();
    ch = 'k';
  }

  //we still process it
  if(inQuote) {
    if(ch.is('"')) {//end quote
      inQuote = false;
      phase = After;
      return EndToken;
    }
    //still inside quotes
    if(phase==Before) {//first char after quote is first care of token
      phase = Inside;
      return BeginToken;
    }
    return Continue;
  }

  switch (phase) {
  case Inside:
    if(ch.isWhite()) {
      phase = After;
      return EndToken;
    }
    switch(ch) {
    case ':': //normal name value seperator
      //we have ended a token and the name,
      phase = Before;
      return EndNameT;

    case '"': //not ours to reason why
      return Illegal;
    case '{': //?missing colon
      return Illegal;
    case '}':
      phase = Before;
      return EndItemT;
    case ',': //normal item seperator
      phase = Before;
      return EndItemT;
    case '\\':
      ++utfFollowers;//abuse this rather than having a dedicated backslash state.
      return Continue;
    default:
      return Continue;
    } // switch
    break;

  case After:
    if(ch.isWhite()) {
      return Continue;
    }
    switch(ch) {
    case ':': //normal name value seperator
      return EndName;
    case '"':
      return Illegal; //maydo: act like missing coma
    case '{':
      return BeginWad;
    case '}':
      return EndWad;
    case ',': //normal item seperator
      return EndItem;
    case '\\':
//      ++utfFollowers;
      return Illegal;
    default:
      phase = Inside;
      return BeginToken;
    } // switch
    break;

  case Before:  //expecting name or value
    if(ch.isWhite()) {
      return Continue;
    }
    switch(ch) {
    case ':':
      return Illegal; //null name is not the same as no name
    case '"': //begin quoted chunk
      inQuote = true;
      return Continue;//but not yet started in chunk, see if(inQuote)
    case '{': //normal
      return BeginWad;
    case '}': //normal
      return EndWad;
    case ',': //sometimes is an extraneous comma, we choose to ignore those.
      return EndItem;//permissive!
    case '\\': //any escape sequence is just like plain text to us
      ++utfFollowers;//abuse this rather than having a dedicated backslash state.
      phase = Inside;
      return BeginToken;
    default:
      phase = Inside;
      return BeginToken;
    } // switch

  } // switch
  return Illegal;// in case we have a missing case above, should never get here.
} // Lexer::next

void Lexer::reset(){
  phase = Before;
  inQuote = false;
  utfFollowers = 0;
}

Action Parser::next(char pushed){
  unsigned mark = location++;//increment before we dig deep lest we forget given the multiple returns.
  if(pushed=='\n') {
    column = 0;
    ++row;
  } else {
    ++column;
  }

  switch (lexer.next(pushed)) {
  case BeginToken: //record location, it is first char of something.
    value.highest = BadIndex; //for safety
    value.lowest = mark;
    return Continue;
  case Continue:   //continue scanning
    return Continue;
  case EndToken:  //just end the token
    endToken(mark);
    return Continue;
  case BeginWad: //open brace encountered
    //caller will inspect haveName
    return BeginWad;
  case EndNameT:  //time to create named node
    endToken(mark);
//    JOIN
  case EndName:  //time to create named node
    name = value;
    value.clear();
    haveName = true;
    return EndName;
  case EndItemT:  //comma between siblings
    endToken(mark);
//    JOIN
  case EndItem:  //comma between siblings
    return EndItem;
  case EndWadT:   //closing wad
    endToken(mark);
//    JOIN
  case EndWad:   //closing wad
    return EndWad;
  case Illegal:    //not a valid char given state, user must decide how to recover.
    return Illegal;
  } // switch
  return Illegal;//should never get here
}

void Parser::itemCompleted(){
  haveName = false;
  name.clear();
  value.clear();
} // Parser::next

void Parser::reset(bool fully){
  lexer.reset();
  itemCompleted();

  if(fully) {
    location = 0;
    row = 0;
    column = 0;
  }
}

Parser::Parser(){
  reset(true);
}

void Parser::endToken(unsigned mark){
  value.highest = mark;
}
