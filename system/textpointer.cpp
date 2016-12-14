#include "textpointer.h"
#include "string.h"
#include "stdlib.h"



TextPointer::TextPointer(): ptr(0){
  //all is well
}

TextPointer::TextPointer(const char *ptr): ptr(0){
  operator =(ptr);
}

TextPointer::~TextPointer(){
  clear();
}

const char *TextPointer::operator =(const char *ptr){
  if(this->ptr != ptr) { //# if not same object (not a content compare)
    clear();
    if(nonTrivial(ptr)) { //todo: see if strcmp is fast enough to use here.
      this->ptr = strdup(ptr);
    }
  }
  return ptr;
}

TextPointer::operator const char *() const {
  return ptr ? : "";
}


bool TextPointer::empty() const {
  return !nonTrivial(ptr);
}

bool TextPointer::is(const char *other) const {
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

bool TextPointer::startsWith(const char *other) const {
  if(ptr == other) {
    return true;
  }
  if(ptr == nullptr) {
    return *other == 0;
  }
  if(other == nullptr) {//all strings start with nothing, for grep '*' case.
    return true;
  }
  //could do the following with strlen and strncmp, but that is more execution. A variant of strcmp which returns the index of mismatch would be handy.
  const char *s(ptr);
  while(char c=*other++){
    char m=*s++;
    if(!m){
      return false;//other is longer than this
    }
    if(c!=*s++){
      return false;//mismatch on existing chars
    }
  }
  return true;
}

void TextPointer::clear(){
  if(ptr) {
    free(const_cast<char *>(ptr));
    ptr = nullptr;
  }
}

char *TextPointer::buffer() const {
  return const_cast<char *>(ptr);
}
