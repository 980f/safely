#include "filename.h"
#include "charformatter.h"

/** no internal slashes, convert to dash */
Text escape (const Text &userinput){
  unsigned len = userinput.length();
  char workspace[Zguard(len)];
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

//static bool isRooted(const Text &simple){
//  return false;// !simple.empty() && simple.at(0)=='/';
//}

FileName::FileName(const Text  &simple):SegmentedName (){
  folder(simple);
}

FileName &FileName::dirname(void){
  this->removeNth(this->quantity()-1);
  return *this;
}

FileName &FileName::folder(const Text  &s){
  if(s.empty()) {
    return *this;
  }
  unsigned quant=this->quantity();//record before parsing
  PathParser::Brackets subracket=PathParser::parseInto(*this,s,'/');
  if(quant==0){
    bracket=subracket;
  } else {
    //ignore there being a leading / in the added piece
    bracket.after = subracket.after;
  }
  return *this;
} // FileName::folder

FileName &FileName::ext(const Text  &s){
  unsigned length=Zguard(s.length()+1);
  Text dotted(length);//+1 for dot.
  CharFormatter catter(dotted.c_str(),length);
  catter.printChar(',');
  catter.cat(s.c_str());
  if(empty()){

  }
  //Text amended()
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
  return PathParser::pack(*this,'/', bracket);
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
