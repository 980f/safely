#include "enumerated.h"

const TextKey Enumerated::InvalidToken = "Undefined";

SequentialEnumeration::SequentialEnumeration(const char *tokenNames[], unsigned numTokens) :
    quantity(numTokens),
    tokenNames(tokenNames) {
  //#done
}

unsigned SequentialEnumeration::numTokens() const {
  return quantity;
}

TextKey SequentialEnumeration::token(unsigned ordinal) const {
  if (ordinal < quantity) {
    return tokenNames[ordinal];
  } else {
    return InvalidToken;
  }
}

unsigned SequentialEnumeration::valueOf(TextKey token) const {
  for (unsigned ordinal = quantity; ordinal-- > 0;) {
    if(same(tokenNames[ordinal], token)) {
      return ordinal;
    }
  }
  return BadIndex;
}
