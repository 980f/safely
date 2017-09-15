#include "textchunk.h"
#include "string.h"

TextBlock::~TextBlock(){
  if(amOwner){
    delete [](&start);
  }
}

int TextBlock::cmp(const char *other) const noexcept {
  return strncmp(&start,other,allocated);
}

bool TextBlock::operator==(const char *other) const noexcept{
  return 0==strncmp(&start,other,allocated);
}

unsigned TextBlock::nextChar(unsigned from, char toseek) const noexcept{
  for(;from<allocated;++from){
    if(at(from)==toseek){
      return from;
    }
  }
  return BadIndex;
}

TextChunk::operator TextBlock() const noexcept{
  Span safe=Span::overlap(span,Span(0,block.allocated));
  if(safe.ordered()){//normal case, a substring
    return TextBlock(&block.at(safe.lowest),span.span());
  } else {
    return TextBlock(nullptr,0);
  }

}
