//"(C) Andrew L. Heilveil, 2017"
#include "halfopen.h"

Span::Span(Index low, Index high):HalfOpen(low,high){}

Span::Span():HalfOpen(BadIndex,BadIndex){}

bool Span::ordered() const {
  if(lowest.isValid()&&highest.isValid()){
    return HalfOpen::ordered();
  } else {
    return false;
  }
}

void Span::leapfrog(unsigned skip){
  lowest=highest+skip;
  highest=BadIndex;
}

void Span::clear(){
  highest=lowest=BadIndex;
}

void Span::shift(unsigned offset){
  //NB: Index -= only modifies the Index if is is valid.
  lowest-=offset;
  highest-=offset;
}

void Span::take(Span &other){
  *this=other;
  other.clear();
}

bool Span::started() const noexcept{
  return lowest.isValid()&& ! highest.isValid();
}
