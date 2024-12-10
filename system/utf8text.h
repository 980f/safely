#pragma once

#include "textpointer.h"
#include "index.h"

/** encode and decode \\u and c-escapes, using Text class */
namespace Utf8Text {
  Text encode(Cstr utf8s, Index presized = BadIndex);

  Text decode(Cstr utf8s, Index presized = BadIndex);

  /** number of bytes for \\u and \\xx and \\tn encoding the given utf8 string. */
  Index encodedLength(Cstr utf8s);

  /** @returns the number of bytes needed for the string if \uxxxx etc are converted to utf8 */
  Index decodedLength(Cstr utf8s);
}
