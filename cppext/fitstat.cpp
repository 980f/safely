//(C) 2017 Andy Heilveil
#include "fitstat.h"
#include "minimath.h"

FitStat::FitStat(){
  init();
}

void FitStat::init(unsigned df) {
  num = 0;
  this->df = df;
  chiSquare = 0.0;
}

FitStat &FitStat::operator=(const FitStat &&other) {
  chiSquare = other.chiSquare;
  df = other.df;
  num = other.num;
  return *this;
}

void FitStat::apply(double deviation){
  ++num;
  chiSquare += squared(deviation);
}
