#include "cstr.h"

#include "string.h"
#include "char.h"
#include "textkey.h"
//todo: wrap stdlib dependent parts with a __have_include__
#include "stdlib.h" //strtod

Cstr::Cstr() : ptr(nullptr){
  //#nada
}

Cstr::Cstr(TextKey target) : ptr(target){

}

Cstr::Cstr(unsigned char *target) : ptr(reinterpret_cast<char *>(target)){
  //#nada
}

TextKey Cstr::operator =(TextKey ptr){
  this->ptr = ptr;
  return ptr;
}

TextKey Cstr::c_str() const noexcept {
  return ptr;
}

const unsigned char *Cstr::raw() const {
  return reinterpret_cast<const unsigned char *>(ptr);
}

const char *Cstr::notNull() const {
  if(ptr) {
    return ptr;
  } else {
    return emptyString;//the const in the return type allows us to point to a byte of read-only memory.
  }
}

bool Cstr::isTrivial() const noexcept {
  return ptr == nullptr || *ptr == 0;
}

const char *Cstr::nullIfEmpty() const {
  if(empty()) {
    return nullptr;
  } else {
    return ptr;
  }
}

Cstr::operator const char *() const {
  return notNull();
}


unsigned Cstr::length() const noexcept {
  return nonTrivial(ptr) ? static_cast<unsigned>(strlen(ptr)) : 0;
}

bool Cstr::endsWith(char isit) const noexcept {
  return empty() ? isit==0 : *this[length() - 1]==isit;
}

bool Cstr::endsWith(TextKey ext) const noexcept {
  Cstr lookfor(ext);
  if(empty()&&lookfor.empty()) {
    return true;
  }
  int offset = length() - lookfor.length();
  if(offset<0) {
    return false;
  }
  if(offset==0) {
    return *this==lookfor;
  }
  Cstr thisend(ptr + offset);
  return thisend ==(lookfor);
} // Cstr::endsWith

bool Cstr::is(TextKey other) const noexcept {
  return same(this->ptr,other);
}

char Cstr::operator [](const Index &index) const noexcept {
  return at(index);
}

char Cstr::at(const Index &index) const noexcept {
  return (nonTrivial(ptr)&&isValid(index)) ? ptr[index] : 0;
}

bool Cstr::setAt(const Index &index, char see) const noexcept {
  if((nonTrivial(ptr)&&isValid(index))) {
    *const_cast<char *>(&ptr[index]) = see;
    return true;
  } else {
    return false;
  }
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
} // Cstr::cmp

bool Cstr::startsWith(TextKey other) const noexcept {
  if(ptr == nullptr) {
    return ::isTrivial(other);
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

bool Cstr::startsWith(char ch) const noexcept {
  return ptr&&*ptr==ch;
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

Index Cstr::trailingZeroes() const {
  unsigned p = length();
  if(p==1) {
    return BadIndex;//easiest way to deal with a single '0' character.
  }
  while(p-->0) {
    if(ptr[p]!='0') {
      if(ptr[p]=='.') {
        return Index(p);//#yes, p not dp. we also remove the dp
      }
      ++p;//point to last zero
      if(p==length()) {
        return BadIndex;//no trailing zeroes.
      }
      //p is the last '0' and is preceded by something other than a '.'
      for(unsigned dp = p; dp-->0;) {
        if(ptr[dp]=='.') {
          //then the trailin zeroes were actually post decimal point
          return Index(p);//#yes, p not dp. we also remove the dp
        }
      }
      return BadIndex;
    }
  }
  return BadIndex;
} // Cstr::trailingZeroes

double Cstr::asNumber(Cstr *tail) const noexcept {
  if(nonTrivial(ptr)) {
    return strtod(ptr, tail ? const_cast<char **>(&tail->ptr) : nullptr);
  } else {
    return 0.0;
  }
}

unsigned Cstr::asUnsigned(const char **tail) const noexcept {
  if (nonTrivial(ptr)) {
    unsigned acc = 0;
    const char *s = ptr;
    while (*s) {
      if (!Char(*s).appliedDigit(acc)) {
        break;
      }
      ++s;
    }
    if (tail) {
      *tail = s;
    }
    return acc;
  } else {
    return 0;
  }
}
void Cstr::clear() noexcept {
  ptr = nullptr;
}

template<> bool Cstr::cvt(bool onNull, Cstr *units) const noexcept {
  if(units) {//COA
    *units = ptr;
  }
  if(nonTrivial(ptr)) {
    if(length()==1) {
      if((*ptr | 1)=='1') {//single decimal 1 or 0
        if(units) {
          *units = nullptr;
        }
        return *ptr & 1;
      }
      //maydo: 't' or 'f'
    }
    if(is("true")) {
      if(units) {
        *units = nullptr;
      }
      return true;
    }
    if(is("false")) {
      if(units) {
        *units = nullptr;
      }
      return false;
    }
    return onNull;
  } else {
    return onNull;
  }
} // Cstr::cvt

template<> long Cstr::cvt(long onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)) {
    return strtol(ptr, units ? const_cast<char **>(&units->ptr) : nullptr,10);
  } else {
    return onNull;
  }
}

template<> unsigned Cstr::cvt(unsigned onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)) {
    return strtoul(ptr, units ? const_cast<char **>(&units->ptr) : nullptr,10);
  } else {
    return onNull;
  }
}

template<> int Cstr::cvt(int onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)) {
    return strtol(ptr, units ? const_cast<char **>(&units->ptr) : nullptr,10);
  } else {
    return onNull;
  }
}

template<> double Cstr::cvt(double onNull, Cstr *units) const noexcept {
  if(nonTrivial(ptr)) {
    return strtod(ptr, units ? const_cast<char **>(&units->ptr) : nullptr);
  } else {
    return onNull;
  }
}
