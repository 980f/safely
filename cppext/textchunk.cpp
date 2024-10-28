#include "textchunk.h"
#include "string.h"

TextBlock::TextBlock(char *start, unsigned allocated, bool amOwner):
  start(*start),
  allocated(allocated),
  amOwner(amOwner)
{

}

TextBlock::~TextBlock(){
  if(amOwner){
    delete [](&start);
  }
}

TextBlock::TextBlock(TextBlock &&other):
  start(other.start),
  allocated(other.allocated),
  amOwner(false)
{

}

TextBlock::TextBlock(const TextBlock &other):
  start(other.start),
  allocated(other.allocated),
  amOwner(false)
{

}

int TextBlock::cmp(const char *other) const noexcept {
  return strncmp(&start,other,allocated);
}

bool TextBlock::operator==(const char *other) const noexcept{
  return 0==strncmp(&start,other,allocated);
}

unsigned TextBlock::nextChar(unsigned from, char toseek) const noexcept{
  for(;from<allocated;++from){
    if(*at(from)==toseek){
      return from;
    }
  }
  return BadIndex;
}

TextChunk::operator TextBlock() const noexcept{
  Span safe=Span::overlap(span,Span(0,block.allocated));
  if(safe.ordered()){//normal case, a substring
    return TextBlock(block.at(safe.lowest),span.span());
  } else {
    return TextBlock(nullptr,0);
  }

}

TextChunk::TextChunk(const TextBlock &block):
  block(block),
  span(){
  //#nada
}

void TextChunk::next(char comma){
//"1" is sizeof(char):
  span.leapfrog(1);//points after previous find, or to start of block
  span.highest=block.nextChar(span.lowest,comma);
}
