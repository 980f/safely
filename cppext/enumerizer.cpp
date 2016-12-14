#include "enumerizer.h"


Enumerizer::Enumerizer(const char **tokenSet, int numTokens): Indexer <TextKey> (tokenSet, numTokens){
  dump();//so that token() works.
}

Enumerizer::Enumerizer(const Enumerizer &other):Indexer <TextKey> (other,false){
  //#always want the entire allocation regardless of state of other
  //#no dump() since we usually do this to get an iterator over the enum.
}

const char *Enumerizer::token(unsigned ordinal) const {
  if(canContain(ordinal)) {//changed to not allow short-sheeting of the enum since the gui stuff is insisting on copying enums at unpredictable times (and hence I can't slip in the fix to call dump()
    return buffer[ordinal];
  }
  return "Undefined";
}

int Enumerizer::ordinal(const char *token){
  Enumerizer scanner(*this);
  while(scanner.hasNext()) {
    const char *canonicalText = scanner.next();
    if(same(canonicalText, token)) {
      return scanner.used() - 1; //next() bumped the ordinal.
    }
  }
  return -1;
} /* ordinal */
//end of file
