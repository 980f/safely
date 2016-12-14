#include "ranged.h"

Ranged::Ranged(){
  //#nada
}

Ranged::Ranged(double highest, double lowest):Range(highest,lowest){
  //#nada
}

Ranged Ranged::around(double center,double factor){
  return Ranged(center*(1+factor),center*(1-factor));
}


double Ranged::clamped(double raw) const { //todo:2 nancheck!
  if(raw < lowest) {
    return lowest;
  }
  if(raw > highest) {
    return highest;
  }
  return raw;
}

bool Ranged::clamp(double &raw) const{
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
u16 Ranged::bin(unsigned numBins, double quantum) const {
  return uround(numBins * ratio((quantum - lowest), width()));
}

double Ranged::map(int fraction, int fullscale)const{
  return lowest+ ratio(width()*fraction,fullscale);
}

double Ranged::map(double fraction)const{
  return lowest+ width()*fraction;
}
