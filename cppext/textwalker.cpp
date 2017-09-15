#include "textwalker.h"
#include "onexit.h"

TextWalker::TextWalker(const TextBlock &source):
  source(source),
  lookahead(nullptr,0),
  done(false){

}

bool TextWalker::hasNext() const {
  return source.span.nonTrivial();
}

TextBlock &TextWalker::next(){
  return lookahead;
}

void TextWalker::skip(unsigned int qty){

}
///////////////
void SimpleTextWalker::seek() {
  source.span.leapfrog(1);//points after previous find, or to start of block

}
