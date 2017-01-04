#include "filename.h"
//limits.h PATH_MAX+1 is the size of the safestr.


/** no internal slashes, conver to %5F */
Text escape (const Text &userinput){
  unsigned len = userinput.length();
  char workspace[len + 1];
  workspace[len] = 0;//before we forget.
  for(unsigned i = 0; i<len; ++i) {
    workspace[i] = userinput[i];
    if(workspace[i]=='/') {
      workspace[i] = '-';
    }
  }
  return Text(workspace);
} // escape

FileName::FileName(){
  //assign(root);
}

static bool isRooted(const Text &simple){
  return false;// !simple.empty() && simple.at(0)=='/';
}

FileName::FileName(const Text  &simple):SegmentedName (){
//  if(!isRooted(simple)) {
//    assign(root);
//  }
//  folder(simple);
}

FileName &FileName::dirname(void){
  this->removeNth(this->quantity()-1);
  return *this;
}

FileName &FileName::folder(const Text  &s,bool escapeit){
  if(s.empty()) {
    return *this;
  }

  return *this;
} // FileName::folder

FileName &FileName::ext(const Text  &s,bool escapeit){
//  assure('.');
//  append(escapeit ? escape(s) : s);
  return *this;
}

FileName &FileName::erase(){
  clear();
  return *this;
}

bool FileName::lastChar(char isit) const {
  if(auto final=last()){
    return final->endsWith(isit);
  } else {
    return false;
  }
}

Text FileName::pack(){
  return pack(PathParser::Brackets());
}

////////////////

NameStacker::NameStacker(FileName &namer, bool escapeit) :
  path(namer),
  mark(namer.quantity()),
  escapeit(escapeit){
}

NameStacker::NameStacker(FileName &namer, const Text &pushsome, bool escapeit) :
  path(namer),
  mark(namer.quantity()),
  escapeit(escapeit){
  path.folder(pushsome, escapeit);
}


NameStacker::~NameStacker(){
  path.clipto(mark);
}
