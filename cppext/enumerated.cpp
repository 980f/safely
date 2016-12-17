#include "enumerated.h"
//#include "uicore.h"
//#include "string.h"
//#include "textkey.h"

const TextKey Enumerated::InvalidToken = "Undefined";

//UString Enumerated::displayName(int value) const{
//  return translate(token(value));
//}

/////////////////////////
SequentialEnumeration::SequentialEnumeration(const char *tokenNames[], int numTokens) : numTokens_(numTokens),tokenNames(tokenNames){
  //finally got them simplified!
}

int SequentialEnumeration::numTokens() const {
  return numTokens_;
}

TextKey SequentialEnumeration::token(int ordinal) const {
  if(0 <= ordinal && ordinal < numTokens_) {
    return tokenNames[ordinal];
  } else {
    return InvalidToken;
  }
}

int SequentialEnumeration::valueOf(TextKey token) const {
  for(int ordinal = numTokens_; ordinal-- > 0; ) {
    if(same(tokenNames[ordinal], token)) {
      return ordinal;
    }
  }
  return -1;
}

Enumerated::~Enumerated(){
}
