#ifndef PUSHEDJSONPARSER_H
#define PUSHEDJSONPARSER_H

/** this class maintains parser state and renders an opinion on characters received.
 * See StoredJSONParser for a user.
 *
 */
#include "halfopen.h"
#include "cheaptricks.h"
namespace PushedJSON {

enum Action {
  Illegal,    //not a valid char given state, user must decide how to recover.
  BeginToken, //record location, it is first char of something.
  Continue,   //continue scanning
  EndToken,  //just end the token
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
  bool haveName;
  unsigned location;//number of chars total
  unsigned row;//number of lines, for error messages
  unsigned column;//where in row
  Lexer lexer;

public://extended return value
  /** used to record extent of a parsed token */

  Span name;
  Span value;

  /**
   *  records locations for text extents, passes major events back to caller
   */
  Action next(char pushed);

  /** called by entity that called next() after it has handled an item, to make sure we don't duplicate items if code is buggy. */
  void itemCompleted();
  /** @param fully is whether to prepare for a new stream, versus just prepare for next item. */
  void reset(bool fully);
  Parser();
private:
  void endToken(unsigned mark);
}; // class Parser

} // namespace PushedJSON
#endif // PUSHEDJSONPARSER_H
