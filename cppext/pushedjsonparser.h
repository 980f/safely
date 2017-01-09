#ifndef PUSHEDJSONPARSER_H
#define PUSHEDJSONPARSER_H

/** this class maintains parser state and renders an opinion on characters received.
 * See StoredJSONParser for a user.
 *
 * It is very tolerant, especially of missing quotes around names and text values.
 * It does not interpret data values, it leaves them as strings as it doesn't actually retain the string in any sense.
 */
#include "halfopen.h"
#include "cheaptricks.h"
namespace PushedJSON {

enum Action {
  Illegal,    //not a valid char given state, user must decide how to recover.
  BeginToken, //record location, it is first char of something.
  Continue,   //continue scanning
  EndToken,  //just end the token
  EndQuoted, //like end token, but of something that was quoted (relevant for null datum)
  BeginWad, //open brace encountered
  EndName,  //time to create named node
  EndNameT,  //time to create named node
  EndItem,  //comma between siblings
  EndItemT,  //comma between siblings
  EndWad,   //closing wad
  EndWadT,   //closing wad
};

class Lexer {
  bool inQuote;
  /** used to skip over utf extended chars */
  CountDown utfFollowers;

  enum Phase {
    Before, //nothing known, expect name or value
    Inside, //inside quotes
    After, //inside unquoted text: we're tolerant so this can be a name or a symbolic value
  } phase;

public:
  /** @returns how to respond to the @param pushed char:
   * if terminateToken then save a pointer to just before where the char came from
   * the Action member indicates what to do with the last completed token, or to start a token
   *
   */
  Action next(char pushed);
  /** in case user wishes to reuse, or recover from a defect */
  void reset();

  Lexer(){
    reset();
  }

}; // class Lexer

/** tracks incoming byte sequence, records interesting points, reports interesting events */
class Parser {
public: // for error messages

  struct Diag {
    unsigned location=0;//number of chars total
  /** number of newelines that have been seen */
    unsigned row=0;//number of lines, for error messages
  /** number of bytes inspected since last newline was seen */
    unsigned column=0;//where in row
  //last char examined, retained for error messages
    char last=0;

    void reset(){
      location = 0;
      row = 0;
      column = 0;
      last=0;
    }

  } d;

  /** parsing is split into token recognition here, and structure definition in the 'Parser' class. */
  Lexer lexer;

public://extended return value

  /** whether a name was seen, bounds recorded in 'name'. */
  bool haveName;
  /** whether the token in progress or previous one if outside a token had qoutes around it. The easiest time to sample it is on an End* event. */
  bool quoted;
  /** 'location' recorded at start and end of name token */
  Span name;
  /** 'location' recorded at start and end of value token. Note that a name is first seen as a value, when it is discoverd to be a name then it is copied into 'name' and is reset. */
  Span value;

  /**
   *  records locations for text extents, passes major events back to caller
   */
  Action next(char pushed);

  /** called by entity that called next() after it has handled an item, to make sure we don't duplicate items if code is buggy. */
  void itemCompleted();
  /** @param fully is whether to prepare for a new stream, versus just prepare for next item. */
  void reset(bool fully);

  /** subtract the @param offset from all values derived from location, including location.
   * this is useful when a buffer is reused, such as in reading a file a line at a time. */
  void shift(unsigned offset);
  Parser();
private:
  void endToken(unsigned mark);
}; // class Parser

} // namespace PushedJSON
#endif // PUSHEDJSONPARSER_H
