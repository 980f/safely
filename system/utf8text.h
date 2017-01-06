#ifndef UTF8TEXT_H
#define UTF8TEXT_H

#include <cstr.h>


/** encode and decode \u and c-escapes, using Text class*/
class Utf8Text {
public:
  Utf8Text();
  /** number of bytes for \u and \xx and \tn encoding the given utf8 string. */
  static unsigned encodedLength(Cstr utf8s);
  /** @returns the number of bytes needed for the string if \uxxxx etc are converted to utf8 */
  static unsigned decodedLength(Cstr utf8s, bool ctoo);
};

#endif // UTF8TEXT_H
