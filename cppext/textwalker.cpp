#include "textwalker.h"
#include "onexit.h"

TextWalker::TextWalker(const TextBlock &source):
  source(source),
  lookahead(nullptr,0),
  done(false){

}

bool TextWalker::hasNext() {
  if(!done &&! source.span.started() ){
    seek();
  }
  return source.span.nonTrivial();
}

void TextWalker::TextWalker::skip(unsigned int qty){
  while(qty-->0){
    seek();
  }
}

TextBlock &TextWalker::next(){
  return lookahead;
}

///////////////
void SimpleTextWalker::seek() {
  source.span.leapfrog(1);//points after previous find, or to start of block
  source.span.highest=source.block.nextChar(source.span.lowest,comma);
}
