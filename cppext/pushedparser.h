#pragma once // "(C) Andrew L. Heilveil, 2017"

#include "halfopen.h" //Span
#include "countdown.h" //ignore trailing chars of a utf8 sequence

/** separate text into chunks of trimmed text and note the non-text chars that separate said chunks.
 * The chars from a stream are pushed at this state machine.
 */
class PushedParser {
public:
  PushedParser();

  virtual ~PushedParser() = default;

  /** items that will result in an 'EndItem' indication */
  const char *seperators = nullptr;

  /** location attributes of an input char, useful for diagnosing formatting errors, but also used to denote boundary of text chunks */
  struct Diag {
    unsigned location = 0; //number of chars total
    /** number of newlines that have been seen */
    unsigned row = 0; //number of lines, for error messages
    /** number of bytes inspected since last newline was seen */
    unsigned column = 0; //where in row
    //last char examined, retained for error messages (and abused for refined view of alternate special chars)
    char last = 0;

    void reset() {
      location = 0;
      row = 0;
      column = 0;
      last = 0;
    }

    void shift(unsigned offset);
  } d;

  bool inQuote = false;
  /** used to skip over utf extended chars */
  CountDown utfFollowers;

  /** lexer states and events
   * <table border="1"> <tbody>
        <tr> <td>from\to</td> <td>Before</td>            <td>Inside</td>       <td>After</td>      </tr>
        <tr> <td>Before</td>  <td>empty item</td>        <td>begin token</td>  <td>--</td>         </tr>
        <tr> <td>Inside</td>  <td>end item and token</td><td>(no event)</td>   <td>end token</td>  </tr>
        <tr> <td>After</td>   <td>end item</td>          <td>--</td>           <td>(no event)</td> </tr>
      </tbody>     </table> */
  enum Phase {
    Before, //nothing known, such as in json expecting name or value
    Inside, //inside quotes
    After, //inside unquoted text: ignore the value of anything other than the endquote
  } phase = Before;

  /** 'location' recorded at start and end of token */
  Span value;
  /** whether value was found with quotes around it */
  bool wasQuoted = false;

  /** tells caller what to do with the token which was terminated by the character passed to next() */
  enum Action {
    //these tend to pass through all layers, to where the character source is:
    Continue, //continue scanning
    Illegal, //not a valid char given state, user must decide how to recover.
    Done, //pass through EOF
    //the next two tend to be lumped together:
    EndItem, //well separated separator
    EndValueAndItem, //separator ended item.

    //no-one seems to care about these events, we'll keep them in case value must be extracted immediately after the terminating character
    BeginValue, //record location, it is first char of something.
    EndValue, //just end the token
  };

  /** assign set of partitioning characters, @returns prior set so that you can stack parsing rules. */
  const char *lookFor(const char *seps);

  /** called by data source, @returns parser state, such as "just recognized the end of a chunk".
   *  records locations for text extents in value, passes major events back to caller
   */
  Action next(char pushed);

  /** called by entity that called next() after it has handled an item, to make sure we don't duplicate items if code or input stream is buggy. */
  void itemCompleted();

  /** @param fully is whether to prepare for a new stream, versus just prepare for next item. */
  void reset(bool fully);

  /** tells this object to subtract @param offset from all values derived from location, including location.
   * this is useful when a buffer is reused, such as in processing a file a block at a time. */
  void shift(unsigned offset);

private:
  Action endValue(bool andItem);

  void beginValue();
};
