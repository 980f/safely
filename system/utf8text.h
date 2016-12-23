#ifndef UTF8TEXT_H
#define UTF8TEXT_H

#include <cstr.h>


/** encode and decode \u and c-escapes, using Text class*/
class Utf8Text
{
public:
  Utf8Text();
  /** number of bytes for \u and \xx and \tn encoding the given utf8 string. */
  static int encodedLength(Cstr utf8s);

  static int decodedLength(Cstr utf8s);
};

#endif // UTF8TEXT_H
