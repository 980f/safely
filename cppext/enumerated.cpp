#include "enumerated.h"
#include "uicore.h"
#include "string.h"
#include "textkey.h"

const TextKey Enumerated::InvalidToken="Undefined";

Glib::ustring Enumerated::displayName(int value) const{
  return translate(token(value));
}

/////////////////////////
SequentialEnumeration::SequentialEnumeration(const char *tokenNames[], int numTokens): simpleTokens(numTokens),tokenNames(tokenNames){
  //finally got them simplified!
}

SequentialEnumeration::~SequentialEnumeration(){
  //obsoleted
}

int SequentialEnumeration::numTokens() const {
  return simpleTokens;
}

TextKey SequentialEnumeration::token(int ordinal) const {
  if(0 <= ordinal && ordinal < simpleTokens) {
    return tokenNames[ordinal];
  } else {
    return InvalidToken;
  }
}

int SequentialEnumeration::valueOf(TextKey token) const {
  for(int ordinal = numTokens(); ordinal-- > 0; ) {
    if(same(tokenNames[ordinal], token)) {
      return ordinal;
    }
  }
  return -1;
}


DynamicEnumeration::DynamicEnumeration(const char *tokenNames[], int tokenNumbers[], int size):
numbers(tokenNumbers),
names(tokenNames),
size(size) {
  //#na
}

int DynamicEnumeration::numTokens() const {
  return size;
}

TextKey DynamicEnumeration::token(int ordinal) const {
  if(0 <= ordinal && ordinal < size) {
    return names[ordinal];
  } else {
    return InvalidToken;
  }
}

int DynamicEnumeration::valueOf(TextKey token) const {
    for(int i=size; i-- > 0; ) {
    if(same(names[i], token)) {
      return i;
    }
  }
  return -1;
}

TextKey DynamicEnumeration::enumToken(int number) const {
  for(int i=0; i<size; ++i){
    if(number==numbers[i]){
      return names[i];
    }
  }
  return nullptr;
}

int DynamicEnumeration::enumValue(TextKey name) const {
  for(int i=0; i<size; ++i){
    if(name==names[i]){
      return numbers[i];
    }
  }
  return -1;
}
