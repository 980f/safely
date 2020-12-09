#include "rangef.h"

Rangef::Rangef(){
  //#nada
}


Rangef Rangef::around(float center,float factor){
  return Rangef(center*(1+factor),center*(1-factor));
}


float Rangef::clamped(float raw) const { //todo:2 nancheck!
  if(raw < lowest) {
    return lowest;
  }
  if(raw > highest) {
    return highest;
  }
  return raw;
}

bool Rangef::clamp(float &raw) const{
  if(raw < lowest) {
    raw=lowest;
    return true;
  }
  if(raw > highest) {
    raw=highest;
    return true;
  }
  return false;
}

/**smallest integer such that array[value in Interval/quantum] won't be an index out of bounds*/
u16 Rangef::bin(unsigned numBins, float quantum) const {
  return uround(numBins * ratio((quantum - lowest), width()));
}

float Rangef::map(int fraction, int fullscale)const{
  return lowest+ ratio(width()*fraction,fullscale);
}

float Rangef::map(float fraction)const{
  return lowest+ width()*fraction;
}