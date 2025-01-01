#pragma once

#include "utf8.h"  //Unichar
#include "countdown.h"

class Unicoder {
public:
  /** while exposed for dealing with errors, it is best to use fetch() to read this guy.*/
  Unichar uch = 0;

  /** */
  enum Event {
    /** push a null, get this back at you */
    End = 0, //in case we get confused and think it is a null char
    /** consuming chars, not emitting anything*/
    More,
    /** finished assembling an escape sequence*/
    Done,
    /** a simple (8 bit) char is ready for you*/
    Plain,
    /** a Unichar is ready for you, and you need to resend the last char, stupid \x stuff. */
    Xand, //this one is only used by Decoder
  };

  virtual Event operator ()(UTF8 ch) =0;

  /** @returns whether state was 'Idle', if not then modifies uch as if a bunch of 0xB0's were seen */
  virtual bool cleanup() =0;

  /** @returns assembled char, clears local copy and state. If you call this when inappropriate you will screw up the parsing.
   * If you are peeking into the state machine's functioning just look at 'uch' directly. */
  Unichar fetch();
  /** @returns slash escape partner of @param afterslash  e.g. newline for "\\n"*/
  static char slashee(char afterslash);

  /** @returns whether @param raw needs slash escaping */
  static bool needsSlash(char raw);

};

/** utf8 to uint32_t push extractor */
class Utf8Escaper : public Unicoder {
public:
  /** remaining utf8 bytes needed to finish assembling a Unicode char */
  CountDown followers = 0;
  /** whether a \-u sequence needs 8 else 4 chars */
  bool bigu = false;

  /** evaluates @param ch and tells you what to do with it. */
  Event operator ()(UTF8 ch) override;

  /** @returns whether state was 'Idle', if not then modifies uch as if a bunch of 0xB0's were seen */
  bool cleanup() override;
};


/** push parser to convert expanded codes back to utf8 */
class Utf8Decoder : public Unicoder {
public:
  /** how many more chars are expected in a \u sequence*/
  CountDown uchers = 0;
  /** how many more chars are expected in a \octal sequence*/
  CountDown octers = 0; //\nnn
  /** whether previous char was a backslash*/
  bool slashing = false;
  /** whether we are processing hex digits until we see one that is not */
  bool xing = false; //\x.....

  Event operator ()(UTF8 ch) override;

  /** @returns whether state was 'Idle', if not then modifies uch as if a bunch of 0xB0's were seen */
  bool cleanup() override;
};
