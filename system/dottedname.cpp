#include "dottedname.h"


DottedName::DottedName(char dot, Cstr initial):bracket(dot){
  parse(initial);
}

void DottedName::parse(const char *rawpath){
  bracket=PathParser::parseInto(*this,rawpath,'.');
}

unsigned DottedName::length(Converter &&cvt)const{
  unsigned pieces=quantity();
  if(pieces==0){
    return 0;
  }
  unsigned bytesNeeded =pieces-1+bracket.before+bracket.after;//number of seperators

  for(auto index(indexer());index.hasNext();){
    bytesNeeded += cvt.length(index.next());
  }
  return bytesNeeded;
}

Text DottedName::pack(Converter &&cvt){
  return PathParser::pack(*this,bracket,cvt.forward());
}