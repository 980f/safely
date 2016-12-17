#include "zstring.h"

#include "string.h"
#include "malloc.h"
#include "textkey.h"

Zstring::Zstring(char *str, bool makeCopy) : str(str),owned(makeCopy){
if (makeCopy){
  if(nonTrivial(str)){
    str=strdup(str);
    if(!str){
      owned=false;//not critical but its nice to be able to breakpoint in incidents like this
    }
  }
}
}

Zstring::Zstring(unsigned len):str(static_cast<char *>(calloc(len+1,1))),owned(true)
{
  if(!str){
    owned=false;//not critical but its nice to be able to breakpoint in incidents like this
  }
}

/** use free() method to reduce impact of some using this after deleted. */
Zstring::~Zstring()
{
 if(owned){
   free();
 }
}

int Zstring::len() const {
  return str ? static_cast<int>(strlen(str)) : 0;
}

char *Zstring::chr(int chr) const {
  if(str) {
    return strchr(str,chr);
  } else {
    return nullptr;
  }
}

int Zstring::cmp(const char *rhs) const {
  if(str) {
    if(rhs) {
      return strcmp(str,rhs);
    } else {
      return *str ? 1 : 0;
    }
  } else {
    return nonTrivial(rhs) ? -1 : 0;
  }
} // Zstring::cmp

bool Zstring::operator ==(const char *rhs) const {
  return same(str,rhs);
}

char *Zstring::cstr() const {
  return str;
}

void Zstring::free(){
  ::free(str);
  str = nullptr;
}
