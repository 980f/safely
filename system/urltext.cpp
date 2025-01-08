#include "urltext.h"

bool UrlText::mustEscape(Char ch){
  return ch.isControl() || ch>=0x7F || ch.in("%/\\ ");
}

unsigned UrlText::encodedLength(const char *it){
  unsigned totes(0);
  while(char ch = (*it++)) {
    if(mustEscape(ch)){
      totes+=3;
    } else {
      ++totes;
    }
  }
  return totes;
}

unsigned UrlText::decodedLength(const char *it){
  CountDown hidgetting;
  unsigned totes(0);
  while(Char ch = *it++) {
    if(hidgetting){
      continue;
    }
    if(ch.is('%')){
      ++totes;
      hidgetting=2;//skip next two chars
    } else {
      ++totes;
    }
  }
  return totes;
}

using namespace UrlText;


const char &Expander::next(){
  if(hidgetting) {
    return hidgets[hidgetting];
  } else {
    Char ch = Indexer::next();
    if(mustEscape(ch)) {
      hidgets[hidgetting = 2] = '%';//in case we lose the const and caller screws up mightily
      hidgets[1] = ch.hexNibble(1);
      hidgets[0] = ch.hexNibble(0);
      return hidgets[hidgetting];
    } else {
      return Indexer::previous();
    }
  }
} // UrlText::next

Expander::Expander(const char *ptr, unsigned length) : Indexer(ptr,length){}

Expander::~Expander(){
}


bool Decoder::push(char incoming){
  Char ch(incoming);
  if(hidgetting){
    //todo: what if char is not a hex digit?
    ch.hexDigit(packer);
    if(hidgetting.isDone()){
      next()=packer;
    }
  } else if(hasNext()){
    if(ch.is('%')){
      packer=0;
      hidgetting=2;
    } else {
      next()=ch;
    }
  }
  return hasNext();
}

Decoder::Decoder(char *ptr, unsigned length): Indexer<char>(ptr, length), packer{0} {}

Decoder::~Decoder(){}
