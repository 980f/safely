#include "urltext.h"

const char &UrlText::next() {
  if(hidget){
    return hidgets[hidget];
  } else {
    Char ch=Indexer::next();
    if(ch.isControl() || ch>=0x7F || ch.in("%/\\ ")){
      hidgets[hidget=2]='%';//in case we lose the const and caller screws up mightily
          hidgets[1]=ch.hexNibble(1);
          hidgets[0]=ch.hexNibble(0);
          return hidgets[hidget];
    } else {
        return Indexer::previous();
      }
    }
  }

  UrlText::UrlText(const char *ptr, unsigned length):Indexer<const char> (ptr,length){

  }

  UrlText::~UrlText(){}
