#ifndef UTF8TRANSCODER_H
#define UTF8TRANSCODER_H

#include "utf8text.h"
#include "utf8.h"  //Unichar
//#include "buffer.h"

/** utf8 to u32 push extractor */
class Utf8Escaper {
public:
  Unichar uch = 0;
  /** remaining utf8 bytes needed to finish assembling a Unicode char */
  CountDown followers = 0;
  /** whether a \u sequence needs 8 else 4 chars */
  bool bigu;

  /** what the most recently inspected char was  */
  enum Event {
    /** give me a null you get one back (End in, End out) */
    End=0, //in case we get confused and think it is a null char
    /** need more chars to figure out what Unichar we have */
    More,
    /** come fetch a unichar, you still have to emit the sequence*/
    Done,
    /** you gave me a passthrough char, for coding convenience it is in uch, you can fetch it.*/
    Plain
  };

  /** evaluates @param ch and tells you what to do with it. */
  Event operator ()( UTF8 ch);

  /** @returns assembled char, clears local copy to expose bugs */
  Unichar fetch();

};


/** push parser to convert expanded codes back to utf8 */
class Utf8Decoder {
public:
  /** while exposed for dealing with errors, it is best to use fetch() to read this guy.*/
  Unichar uch = 0;
  /** how many more chars are expected in a \u sequence*/
  CountDown uchers = 0;
  /** how many more chars are expected in a \octal sequence*/
  CountDown octers = 0; //\nnn
  /** whether previous char was a backslash*/
  bool slashing = false;
  /** whether we are processing hex digits until we see one that is not */
  bool xing = false; //\x.....

  /** */
  enum Event {
    /** push a null, get this back at you */
    End=0, //in case we get confused and think it is a null char
    /** consuming chars, not emitting anything*/
    More,
    /** finished assembling an escape sequence*/
    Done,
    /** a simple (8 bit) char is ready for you*/
    Plain,
    /** a Unichar is ready for you, and you need to resend the last char, stupid \x stuff. */
    Xand,
  };

  Event operator ()( UTF8 ch);

  /** @returns assembled char, clears local copy to expose bugs */
  Unichar fetch();

};


/** @returns s;ash escape partner of @param afterslash */
char slashee(char afterslash);
bool needsSlash(char raw);


#endif // UTF8TRANSCODER_H
