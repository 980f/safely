#include "filename.h"
#include "charformatter.h"

#include <malloc.h>

FileName::FileName(){
  //assign(root);
}

FileName::FileName(const Text  &simple){
  folder(simple);
}

FileName &FileName::dirname(void){
  removeLast();
  return *this;
}

FileName &FileName::folder(const Text  &s){
  if(s.empty()) {
    return *this;
  }
  unsigned quant=this->quantity();//record before parsing



//  PathParser::Rules subracket=PathParser::parseInto(*this,s,'/');
//  if(quant==0){
//    bracket=subracket;
//  } else {
//    //ignore there being a leading / in the added piece
//    bracket.after = subracket.after;
//  }

  return *this;
} // FileName::folder

FileName &FileName::ext(const Text  &s){
  if(empty()){//becomes totality
    auto fname=new DottedName('.',s);
    fname->bracket.before=true;
    append(fname);
  } else {
    auto fname=last();
    fname->suffix(s.c_str());
  }
  return *this;
}

FileName &FileName::erase(){
  clear();
  return *this;
}

unsigned FileName::length(Converter &&cvt) const {
  unsigned pieces=quantity();
  if(pieces==0){
    return 0;
  }

  unsigned bytesNeeded =pieces-1+bracket.before+bracket.after;//number of seperators

  for(ConstChainScanner<DottedName> index(*this);index.hasNext();){
    bytesNeeded += index.next().length(cvt.forward());
  }
  return bytesNeeded;
}


Text FileName::pack(Converter &&cvt,unsigned bytesNeeded){
  if(!Index(bytesNeeded).isValid()){
    bytesNeeded=length(cvt.forward());
  }
  Indexer<char> packer=Indexer<char>::make(bytesNeeded,true);

  for(ChainScanner<DottedName> feeder(*this);feeder.hasNext();) {
    if(feeder.ordinal()>0 || bracket.before){//if not first or if put before first
      packer.next() = bracket.slash;
    }

  }
  if(bracket.after && packer.used()>0) {//only append trailing slash if there is something ahead of it
    packer.next() = bracket.slash;
  }
  //and in case we overestimated the length needed:
  packer.next()=0;//null terminate since we didn't pre-emptively calloc.

  return Text(packer.internalBuffer());//when you destroy the Text the data malloc'd above is freed
}

////////////////

NameStacker::NameStacker(FileName &namer) :
  path(namer),
  mark(namer.quantity()){
}

NameStacker::NameStacker(FileName &namer, const Text &pushsome) :
  path(namer),
  mark(namer.quantity()){
  path.folder(pushsome);
}


NameStacker::~NameStacker(){
  path.clipto(mark);
}
