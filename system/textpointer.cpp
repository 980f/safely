#include "textpointer.h"
#include "string.h"
#include "stdlib.h"


TextPointer::TextPointer() : ptr(0){
  //all is well
}

TextPointer::TextPointer(const char *ptr) : ptr(0){
  operator =(ptr);
}

TextPointer::~TextPointer(){
  clear();
}

TextKey TextPointer::operator =(TextKey ptr){
  if(this->ptr != ptr) { //# if not same object (not a content compare)
    clear();
    if(nonTrivial(ptr)) {
      this->ptr = strdup(ptr);
    }
  }
  return ptr;
}

TextPointer::operator const char *() const {
  return ptr ?: "";
}

bool TextPointer::empty() const {
  return !nonTrivial(ptr);
}

int TextPointer::length() const {
  return nonTrivial(ptr) ? strlen(ptr) : 0;
}

bool TextPointer::is(TextKey other) const {
  if(ptr == other) {//same object
    return true;
  }
  if(ptr == nullptr) {// null pointer matches empty string
    return *other == 0;
  }
  if(other == nullptr) {//empty string matches null pointer
    return *ptr == 0;
  }
  if(0 == strcmp(ptr, other)) {
    return true;
  } else {
    return false;
  }
} // is

bool TextPointer::startsWith(TextKey other) const {
  if(ptr == nullptr) {
    return other==nullptr || *other == 0;
  }
  if(ptr == other) {
    return true;
  }
  if(other == nullptr) {//all strings start with nothing, for grep '*' case.
    return true;
  }
  //could do the following with strlen and strncmp, but that is more execution. A variant of strcmp which returns the index of mismatch would be handy.
  const char *s(ptr);
  while(char c = *other++) {
    char m = *s++;
    if(!m) {
      return false;//other is longer than this
    }
    if(c!=m) {
      return false;//mismatch on existing chars
    }
  }
  return true;
} // TextPointer::startsWith

void TextPointer::clear(){
  if(ptr) {//checking for debug
    free(const_cast<char *>(ptr));
    ptr = nullptr;
  }
}

char *TextPointer::buffer() const {
  return const_cast<char *>(ptr);
}
