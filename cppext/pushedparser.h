#pragma once // "(C) Andrew L. Heilveil, 2017"

#include "halfopen.h" //Span

/** separate text into chunks of trimmed text and note the non-text char that separate said chunks.
 * The chars from a stream are pushed at this state machine.
 *
 * Usage:
 * on char received:
 *   push char to decoder, if "have char" then push that to this parser.
 *   if parser return Done then its span field tells you where in your text stream the value is, d.last is the actual separator (the last char you pushed).
 *   When you have secured the value you may choose to shift() in order to minimize how much lookback memory you need for values.
 *   The parser returns Continue for all characters that are not in separators. It records first position that is not in trimmers.
 *   It will not trim trailing trimmers until it sees a separator, it stretches the span to the last "not trimmer" while waiting for a separator.
 *   phase can tell you if a body is detected but not yet terminated.
 *   You will get illegal if you push a null and there is an open quote, and perhaps other corner cases.
 *
 * call next() until you get a Done then reset(false) for normal cases or reset(true) to erase all state, to lose all memory of what has happened.
 */
class PushedParser {
public:
  PushedParser();

  virtual ~PushedParser() = default;

  struct Ruleset {
    /** items that will result in an 'EndItem' indication */
    const char *seperators = nullptr;
    /** having null be the same as a separator is very nice for csv */
    bool nullIsSeperator = false;
    /** exclusion for chars at the start and end of the span */
    const char *trimmers = nullptr;
    /* sometimes your token can include framing, this string is pairs of open and close brackets to recognize and discard.*/
    const char *quoters = nullptr;

    void typicalCsv() {
      seperators = ",\n";
      nullIsSeperator = true;
      trimmers = " \t\r\n";//separators takes priority over trimmers
      quoters = "\"\"''";
    }

    void typicalTab() {
      seperators = "\t\n";
      nullIsSeperator = true;
      trimmers = " \r\n";//separators takes priority over trimmers
      quoters = "\"\"''";
    }

    void egCalculator() {
      seperators = ",+-=/*^%\n";
      nullIsSeperator = true;
      trimmers = " \t\r\n";//separators takes priority over trimmers
      quoters = "[]{}()";
    }
  } rule;

  /** 'location' recorded at start and end of token */
  Span span;
  /** quote state has been augmented from prior version of this lib to allow for `' matching, <> matching and the like. */
  char endQuote = 0; //if not null then is char that ends quoted segment.
  char wasQuote = 0; //a note to caller of what kind of quote was removed.

  enum Phase {
    Before, //nothing known, such as in json expecting name or value
    Inside, //inside quotes, 'not trimmer' has been seen. Might replacethis with state of span.
    After, //inside unquoted text: ignore the value of anything other than the endquote
  };;


  /** tells caller what to do with the token which was terminated by the character passed to next() */
  enum Action {
    //these tend to pass through all layers, to where the character source is:
    Continue, //continue scanning
    Illegal, //not a valid char given state, user must decide how to recover.
    Done, //item has been found
  };

  /** called by data source, @returns parser state, such as "just recognized the end of a chunk".
   *  records locations for text extents in value, passes major events back to caller
   */
  Action next(char pushed);

  /** called by entity that called next() after it has handled an item, to make sure we don't duplicate items if code or input stream is buggy.
     @param fully is whether to prepare for a new stream, versus just prepare for next item. */
  void reset(bool fully);

  /** tells this object to subtract @param offset from all values derived from location, including location.
   * this is useful when a buffer is reused, such as in processing a file a block at a time. */
  void shift(unsigned offset);

  /** location attributes of an input char, useful for diagnosing formatting errors */
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

    /** update internal to track text statistics of input position */
    void apply(char pushed);

    void shift(unsigned offset);
  } d;

private:
  Action endValue();
};
