#pragma once
//"(C) Andrew L. Heilveil, 2017"

/** this class maintains parser state and renders an opinion on characters received.
 * See StoredJSONParser for a user.
 *
 * It is very tolerant, especially of missing quotes around names and text values.
 * It does not interpret data values, it leaves them as strings as it doesn't actually retain the string in any sense.
 */
#include "pushedparser.h"
#include "onexit.h"

#include "extremer.h"

namespace PushedJSON {

  struct Stats {
    /**number of values */
    unsigned totalNodes = 0;
    /**number of terminal values */
    unsigned totalScalar = 0;
    /** greatest depth of nesting */
    SimpleExtremer<unsigned,true,false> maxDepth;
    /** number of unmatched braces at end of parsing. If massive then more closes than opens. */
    unsigned nested = 0;

    /** call when you push to a child */
    void nest();
    /** call when you complete a child */
    void popnest();
    /** prepare for fresh use */
    void reset();

    /** update node counters, @param scalar should be true for just leaf nodes. */
    void onNode(bool scalar);

    class DepthTracker:public CountedLock {
    public:
      DepthTracker(Stats &s);
    };

  };

enum Action {
  Continue,   //continue scanning
  Illegal,    //not a valid char given state, user must decide how to recover.
  EndItem,  //comma between siblings
  BeginWad, //open brace encountered
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

  Span value;
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
  bool recordName();
  void recordItem(unsigned mark);
}; // class Parser


} // namespace PushedJSON
