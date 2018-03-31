#include "enumerizer.h"


Enumerizer::Enumerizer(TextKey tokenSet[], int quantity) : Indexer<TextKey>(tokenSet, quantity){
  dump();//so that token() works, the constructor of the Indexer indicates all are empty but we know that none are.
}

Enumerizer::Enumerizer(const Enumerizer &other) : Indexer<TextKey>(other,false){
  //#always want the entire allocation regardless of state of other
  //#no dump() since we usually do this to get an iterator over the enum.
}

const char *Enumerizer::token(unsigned ordinal) const {
  if(canContain(ordinal)) {//changed to not allow short-sheeting of the enum since the gui stuff is insisting on copying enums at
                           // unpredictable times (and hence I can't slip in the fix to call dump()
    return buffer[ordinal];
  }
  return "Undefined";
}

unsigned Enumerizer::ordinal(const char *token){
  Enumerizer scanner(*this);
  while(scanner.hasNext()) {
    const char *canonicalText = scanner.next();
    if(same(canonicalText, token)) {
      return scanner.used() - 1; //next() bumped the ordinal.
    }
  }
  return BadIndex;
} /* ordinal */

//end of file
