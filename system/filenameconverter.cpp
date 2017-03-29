#include "filenameconverter.h"


#include "cstr.h"
#include "char.h"
static Cstr escapees("*?:");
unsigned FileNameConverter::length(const char *source) const{
  Cstr s(source);
  Indexer<const char>str(s,s.length());
  unsigned expanded=str.allocated();
  while(str.hasNext()){
    char c=str.next();
    if(escapees.index(c)!=BadIndex){
      expanded+=2;
    }
  }
  return expanded;
}

bool FileNameConverter::operator()(const char *source, Indexer<char> &packer){
  Cstr s(source);
  Indexer<const char>str(s,s.length());

  while(str.hasNext()&&packer.hasNext()){
    Char c=str.next();
    if(escapees.index(c)!=BadIndex){
      if(packer.stillHas(3)){
      packer.next()='%';
      packer.next()=c.hexNibble(1);
      packer.next()=c.hexNibble(0);
      } else {
        return false;
      }
    } else {
      packer.next()=c;
    }
  }
  return !str.hasNext();
}
