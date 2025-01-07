//"(C) Andrew L. Heilveil, 2017"
#include "safely.h"
#include "pushedjsonparser.h"
#include "onexit.h"

using namespace PushedJSON;


/* Algorithm
 * switch (separator)
 * case ':' (or '=') if field isTrivial report error else record as name, if already named then error else proceed
 * case '{' or '[' if field is notTrivial report error else create and push new wad (call agent's new-wad method)
 * case ']' or '}' if field is notTrivial call add_child then call wad complete
 * case ',' or '\n' call add_child , add_child gets to decide whether a trivial value is acceptible.
 *
 */

bool Parser::recordName() {
  if (haveName) {
    return false;
  }
  name.take(value);
  quotedName = wasQuote != 0; //lose type of quote
  haveName = true; //todo:0 qualify this on nonTrivial name, but then we have to deal with "" as a name :(
  return true; //no fault, not that we have a name.
}

void Parser::recordItem(unsigned mark) {}

Action Parser::next(char pushed) {
  switch (PushedParser::next(pushed)) {
    case Continue:
      return PushedJSON::Continue;

    case Illegal:
      return PushedJSON::Illegal;
    case Done:
      switch (d.last) {
        case '=':
          if (!rule.equalscolon) {
            return PushedJSON::Illegal; //might soften this to allow '=' inside values
          }
          d.last = ':'; //alter token so that downstream guys don't need to have rule.equalscolon access
        //else
          JOIN
        case ':':
          if (!recordName()) {
            return PushedJSON::Illegal;
          }
          return PushedJSON::Continue; //null name is not the same as no name
        case '[': //array
          orderedWad = true;
          return BeginWad; //caller picks up name and does creation etc. of wad with pushing et al.
        case '{': //normal
          orderedWad = false;
          return BeginWad;
        case ']':
          orderedWad = true;
          return EndWad; //if value then add_child before popping wad, if name and no value return illegal?
        case '}': //normal
          orderedWad = false;
          return EndWad;
        case ';':
          if (!rule.semicomma) {
            return PushedJSON::Illegal;
          }
          JOIN
        case ',': //sometimes is an extraneous comma, we choose to ignore those.
          return EndItem; //missing value, possible missing whole child.
        case '\n': //potential separate this out and report "missing comma"

          return EndItem;
        case 0: /*abnormal*/
          return EndItem; //ok for single value files, not so much for normal ones.
        default:
          return PushedJSON::Illegal;
      } // switch
  }
  return PushedJSON::Illegal; //catch regressions
}

void Parser::itemCompleted() {
  haveName = false;
  quotedName = false;
  name.clear();
  value.clear();
  PushedParser::reset(false);
} // Parser::next

void Parser::reset(bool fully) {
  PushedParser::reset(fully);
  itemCompleted();
}

void Parser::shift(unsigned offset) {
  if (haveName) { //gratuitous test, name can shift for itself even when not valid.
    name.shift(offset);
  }
  PushedParser::shift(offset);
}


Parser::Parser() {
  PushedParser::rule.seperators=":{,}[]\n"; //Andy's lax json parsing
  PushedParser::rule.nullIsSeperator = true;//same as a comma, this might bw a bad idea.
  PushedParser::rule.trimmers = " \t\r";
  PushedParser::rule.quoters = "\"\"''"; //allowing single or double quotes for string deliminator
  reset(true);
}

Stats::DepthTracker::DepthTracker(Stats &s): CountedLock(s.nested) { //increments nested , when this tracker is deleted CountedLock deletion will decrement nested.
  s.maxDepth.inspect(s.nested);
}

void Stats::nest() {
  maxDepth.inspect(++nested);
}

void Stats::popnest() {
  --nested;
}

void Stats::reset() {
  totalNodes = 0;
  totalScalar = 0;
  maxDepth.reset();
  nested = 0;
}

void Stats::onNode(bool scalar) {
  ++totalNodes;
  if (scalar) {
    ++totalScalar;
  }
}
