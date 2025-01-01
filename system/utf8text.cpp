#include "utf8text.h"
#include "buffer.h"
#include "utf8converter.h"

//utf8 -> uint32_t -> \u or \U
Text Utf8Text::encode(Cstr utf8s, Index presized) {
  if (!presized.isValid()) {
    presized = encodedLength(utf8s);
    if (!presized.isValid()) {
      return Text();
    }
  }
  Text retval(Zguard(presized));
  Utf8ConverterOut ex; //as in ex_pander
  Indexer<char> packer(retval.violated(), presized);
  ex(utf8s.c_str(), packer);
  return retval;
}

Index Utf8Text::encodedLength(Cstr utf8s) {
  return Utf8ConverterOut().length(utf8s);
} // Utf8Text::encodedLength


Index Utf8Text::decodedLength(Cstr utf8s) {
  return Utf8ConverterIn().length(utf8s);
}

Text Utf8Text::decode(Cstr utf8s, Index presized) {
  if (!presized.isValid()) {
    presized = decodedLength(utf8s);
    if (!presized.isValid()) {
      return Text();
    }
  }
  Text retval(Zguard(presized));
  Utf8ConverterIn dx;
  Indexer<char> packer(retval.violated(), presized);
  dx(utf8s.c_str(), packer);
  return retval;
}
