#include "enumerated.h"
<<<<<<< HEAD
#include "index.h"
=======
>>>>>>> dp5qcu

//using namespace Safely;

<<<<<<< HEAD
const TextKey Enumerated::InvalidToken = "Undefined";

=======
>>>>>>> dp5qcu
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
<<<<<<< HEAD
    if (same(tokenNames[ordinal], token)) {
=======
    if(same(tokenNames[ordinal], token)) {
>>>>>>> dp5qcu
      return ordinal;
    }
  }
  return BadIndex;
}
