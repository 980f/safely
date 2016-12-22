#include "cstr.h"

#include "string.h"

Cstr::Cstr() : ptr(nullptr){
  //#nada
}

Cstr::Cstr(TextKey target) : ptr(target){
  //#nada
}

Cstr::~Cstr(){
  //#need explicit instantiation for vtable to get emitted.
}

TextKey Cstr::operator =(TextKey ptr){
  this->ptr = ptr;
  return ptr;
}

TextKey Cstr::c_str() const {
  return ptr;
}

Cstr::operator const char *() const {
  return ptr ? ptr : emptyString;//the const in the return type allows us to point to a byte of read-only memory.
}

bool Cstr::empty() const {
  return isTrivial(ptr);
}

int Cstr::length() const {
  return nonTrivial(ptr) ? static_cast<int>(strlen(ptr)) : 0;
}

bool Cstr::is(TextKey other) const {
  return same(this->ptr,other);
}

/** attempt to match the reasoning of the @see same() function with respect to comparing null strings and empty strings */
int Cstr::cmp(TextKey rhs) const {
  if(ptr) {
    if(rhs) {
      return strcmp(ptr,rhs);
    } else {//rhs is nullptr
      return *ptr ? 1 : 0;
    }
  } else {//this wraps nullptr
    return nonTrivial(rhs) ? -1 : 0;
  }
} // Zstring::cmp

bool Cstr::startsWith(TextKey other) const {
  if(ptr == nullptr) {
    return isTrivial(other);
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
} // Cstr::startsWith

int Cstr::index(char ch) const {
  if(const char *candidate = chr(ch)) {
    return candidate - ptr;
  } else {
    return -1;
  }
}

int Cstr::rindex(char ch) const {
  if(const char *candidate = rchr(ch)) {
    return candidate - ptr;
  } else {
    return -1;
  }
}

const char *Cstr::chr(int chr) const {
  if(nonTrivial(ptr)) {
    return strchr(ptr,chr);
  } else {
    return nullptr;
  }
}

const char *Cstr::rchr(int chr) const {
  if(nonTrivial(ptr)) {
    return strrchr(ptr,chr);
  } else {
    return nullptr;
  }
} // TextPointer::startsWith

void Cstr::clear(){
  ptr = nullptr;
}
