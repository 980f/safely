#include "zstring.h"

#include "string.h"
#include "malloc.h"
#include "textkey.h"

Zstring::Zstring(char *str, bool makeCopy) : str(str),owned(makeCopy){
  if (makeCopy) {
    if(nonTrivial(str)) {
      str = strdup(str);
      if(!str) {
        owned = false; //not critical but its nice to be able to breakpoint in incidents like this
      }
    }
  }
}

Zstring::Zstring(unsigned len) : str(static_cast<char *>(calloc(len + 1,1))),owned(true){
  if(!str) {
    owned = false; //not critical but its nice to be able to breakpoint on attempts to wrap a nullptr
  }
}

/** uses free() method to [reduce impact/make uniform crash out] of someone using this after deleted. */
Zstring::~Zstring(){
  if(owned) {
    free();
  }
}

int Zstring::len() const {
  return notNull() ? static_cast<int>(strlen(str)) : 0;
}

char *Zstring::chr(int chr) const {
  if(notNull()) {
    return strchr(str,chr);
  } else {
    return nullptr;
  }
}

/** attempt to match the reasoning of the @see same() function with respect to comparing null strings and empty strings */
int Zstring::cmp(const char *rhs) const {
  if(notNull()) {
    if(rhs) {
      return strcmp(str,rhs);
    } else {//rhs is nullptr
      return *str ? 1 : 0;
    }
  } else {//this is nullptr
    return nonTrivial(rhs) ? -1 : 0;
  }
} // Zstring::cmp

bool Zstring::operator ==(const char *rhs) const {
  return notNull() && same(str,rhs);
}

void Zstring::clear(){
  if(this!=nullptr) {
    if(owned) {
      free();
    } else {
      str = nullptr;
    }
  }
}

Zstring &Zstring::copy(const Zstring &other){
  if(notNull()) {
    clear();
    if(nonTrivial(other.str)) {
      str = strdup(other.str);
      owned = str!=nullptr;
    }
  }
  return *this;
}

char *Zstring::c_str() const {
  return notNull() ? str : nullptr;
}

void Zstring::free(){
  if(notNull()) {
    ::free(str);
    str = nullptr;
  }
}
