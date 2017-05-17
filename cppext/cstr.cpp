#include "cstr.h"
#include "safely.h"
#include "string.h"
#include "stdlib.h" //strtod

Cstr::Cstr() : ptr(nullptr){
  //#nada
}

Cstr::Cstr(TextKey target):ptr(target){

}

Cstr::Cstr(unsigned char *target) : ptr(reinterpret_cast<char *>(target)){
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

const unsigned char *Cstr::raw() const {
  return reinterpret_cast<const unsigned char *>(ptr);
}

const char *Cstr::notNull() const {
  if(ptr){
    return ptr;
  } else {
    return emptyString;//the const in the return type allows us to point to a byte of read-only memory.
  }
}

const char *Cstr::nullIfEmpty() const {
  if(empty()){
    return nullptr;
  } else {
    return ptr;
  }
}

Cstr::operator const char *() const {
  return notNull();
}

bool Cstr::empty() const {
  return isTrivial(ptr);
}

unsigned Cstr::length() const noexcept {
  return nonTrivial(ptr) ? static_cast<unsigned>(strlen(ptr)) : 0;
}

bool Cstr::endsWith(char isit) const noexcept {
  return *this[length()-1]==isit;
}

bool Cstr::is(TextKey other) const noexcept {
  return same(this->ptr,other);
}

char Cstr::operator [](const Index &index) const noexcept {
  return (nonTrivial(ptr)&&isValid(index)) ? ptr[index]:0;
}

/** attempt to match the reasoning of the @see same() function with respect to comparing null strings and empty strings */
int Cstr::cmp(TextKey rhs) const noexcept {
  if(ptr) {
    if(rhs) {
      return strcmp(ptr,rhs);
    } else {//rhs is nullptr
      return *ptr ? 1 : 0;
    }
  } else {//this wraps nullptr
    return nonTrivial(rhs) ? -1 : 0;
  }
}

bool Cstr::startsWith(TextKey other) const noexcept {
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

Index Cstr::index(char ch) const noexcept {
  if(const char *candidate = chr(ch)) {
    return candidate - ptr;
  } else {
    return BadIndex;
  }
}

Index Cstr::rindex(char ch) const noexcept {
  if(const char *candidate = rchr(ch)) {
    return candidate - ptr;
  } else {
    return BadIndex;
  }
}

const char *Cstr::chr(int chr) const noexcept {
  if(nonTrivial(ptr)) {
    return strchr(ptr,chr);
  } else {
    return nullptr;
  }
}

const char *Cstr::rchr(int chr) const noexcept {
  if(nonTrivial(ptr)) {
    return strrchr(ptr,chr);
  } else {
    return nullptr;
  }
}

double Cstr::asNumber(Cstr *tail) const noexcept{
  if(nonTrivial(ptr)){
    return strtod(ptr, tail ? const_cast<char **>(&tail->ptr) : nullptr);
  } else {
    return 0.0;
  }
}

void Cstr::clear() noexcept{
  ptr = nullptr;
}

template<> long Cstr::cvt(long onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)){
    return strtol(ptr, units? const_cast<char **>(&units->ptr) : nullptr,10);
  } else {
    return onNull;
  }
}

template<> unsigned Cstr::cvt(unsigned onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)){
    return strtoul(ptr, units? const_cast<char **>(&units->ptr) : nullptr,10);
  } else {
    return onNull;
  }
}

template<> int Cstr::cvt(int onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)){
    return strtol(ptr, units? const_cast<char **>(&units->ptr) : nullptr,10);
  } else {
    return onNull;
  }
}



template<> double Cstr::cvt(double onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)){
    return strtod(ptr, units? const_cast<char **>(&units->ptr) : nullptr);
  } else {
    return onNull;
  }
}
