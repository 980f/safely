#include "halfopen.h"

Span::Span(unsigned low, unsigned high):HalfOpen(low,high){}

Span::Span():HalfOpen(BadIndex,BadIndex){}

Span::~Span(){}

bool Span::ordered() const {
  if(isValid(lowest)&&isValid(highest)){
    return HalfOpen::ordered();
  } else {
    return false;
  }
}

void Span::leapfrog(unsigned skip){
  lowest=highest+skip;
  highest=BadIndex;
}
