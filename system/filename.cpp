//(C) 2017 Andrew Heilveil
#include "filename.h"

FileName::FileName(){
  //#nada
}

FileName::FileName(TextKey simple){
  parse(simple);
}

FileName::FileName(const Text  &simple){
  parse(simple);
}

FileName &FileName::dirname(void){
  removeLast();
  return *this;
}

//FileName &FileName::folder(const Text  &s){
//  if(s.empty()) {
//    return *this;
//  }


//  return *this;
//} // FileName::folder

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

FileName &FileName::parse(const char *rawpath){
  Cstr s(rawpath);
  if(!s.empty()){
    Indexer<const char> buffer(s.c_str(),s.length());
    PathParser::Chunker chunker('/');
    unsigned leaders=chunker.start(buffer)>0;//purge leading slashes on all segments
    if(empty()){//but if first record that they existed.
      chunker.bracket.before=leaders>0;
    }
    while(Span span=chunker.next(buffer)){
      Indexer<const char> segment(buffer.view(span.lowest,span.highest));
      DottedName *folder=new DottedName('.',segment);
      append(folder);
    }
    bracket=chunker.bracket;
  }
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
  path.parse(pushsome);
}


NameStacker::~NameStacker(){
  path.clipto(mark);
}
