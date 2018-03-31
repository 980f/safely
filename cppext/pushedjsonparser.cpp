//"(C) Andrew L. Heilveil, 2017"
#include "safely.h"
#include "pushedjsonparser.h"
#include "utf8.h"
#include "localonexit.h"

using namespace PushedJSON;

void Parser::recordName(){
  name.take(value);
  quotedName=take(wasQuoted);
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
    case '=':
      if(!rule.equalscolon){
        return PushedJSON::Illegal;//might soften this to allow '=' inside values
      }
      d.last=':';//alter token so that downstream guys don't need to have rule.equalscolon access
      //else
      //JOIN
    case ':':
      recordName();
      return PushedJSON::Continue; //null name is not the same as no name
    case '[': //array
      orderedWad=true;
      return BeginWad;
    case '{': //normal
      orderedWad=false;
      return BeginWad;
    case ']':
      orderedWad=true;
      return EndWad;
    case '}': //normal
      orderedWad=false;
      return EndWad;
    case ';':
      if(rule.semicomma){
        d.last=',';//canonical token
        return PushedJSON::EndItem;//missing value, possible missing whole child.
      } else {
        return PushedJSON::Illegal;
      }

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
  lookFor(StandardJSONFraming);
  reset(true);
}



JsonStats::DepthTracker::DepthTracker(JsonStats &s):
  CountedLock(s.nested){//increments nested , when this tracker is deleted CountedLock deletion will decrement nested.
  s.maxDepth.inspect(s.nested);
}

void JsonStats::nest(){
  maxDepth.inspect(++nested);
}

void JsonStats::popnest(){
  --nested;
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
