#ifndef PUSHEDJSONPARSER_H
#define PUSHEDJSONPARSER_H

/** this class maintains parser state and renders an opinion on characters received.
 * See StoredJSONParser for a user.
 *
 * It is very tolerant, especially of missing quotes around names and text values.
 * It does not interpret data values, it leaves them as strings as it doesn't actually retain the string in any sense.
 */
#include "halfopen.h"
#include "cheaptricks.h" //CountDown
#include "pushedparser.h"
#include "localonexit.h"

#include "extremer.h"
struct JsonStats {
  /**number of values */
  unsigned totalNodes = 0;
  /**number of terminal values */
  unsigned totalScalar = 0;
  /** greatest depth of nesting */
  SimpleExtremer<unsigned> maxDepth;
  /** number of unmatched braces at end of parsing */
  unsigned nested = 0;

  void reset();

  void onNode(bool scalar);

  class DepthTracker:public CountedLock {
  public:
    DepthTracker(JsonStats &s);
  };

};

namespace PushedJSON {

enum Action {
  Illegal,    //not a valid char given state, user must decide how to recover.
  Continue,   //continue scanning
  BeginWad, //open brace encountered
  EndItem,  //comma between siblings
  EndWad,   //closing wad
  Done
};

/** tracks incoming byte sequence, records interesting points, reports interesting events */
class Parser:public PushedParser {

public://extended return value
  struct Rules {
    //treat semicolons as item terminator
    bool semicomma=false;
    bool equalscolon=false;
  } rule;

  /** whether a name was seen, bounds recorded in 'name'. */
  bool haveName;
  /** 'location' recorded at start and end of name token */
  Span name;
  bool quotedName;
  /** our first user doesn't care about the difference between [ and {, but someone else may so: */
  bool orderedWad;
  /**
   *  records locations for text extents, passes major events back to caller
   */
  PushedJSON::Action next(char pushed);

  /** to be called by agent that called next() after it has handled an item, to make sure we don't duplicate items if code is buggy. */
  void itemCompleted();

  /** @param fully is whether to prepare for a new stream, versus just prepare for next item. */
  void reset(bool fully);

  /** subtract the @param offset from all values derived from location, including location.
   * this is useful when a buffer is reused, such as in reading a file a line at a time. */
  void shift(unsigned offset);

  Parser();

private:
  void recordName();
  void endToken(unsigned mark);
}; // class Parser

#define StandardJSONFraming ":{,}[]"

} // namespace PushedJSON
#endif // PUSHEDJSONPARSER_H
