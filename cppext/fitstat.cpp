#include "fitstat.h"
#include "minimath.h"
FitStat::FitStat(){
  init();
}

void FitStat::init(int df){
  num = 0;
  this->df = df;
  chiSquare = 0.0;
}

FitStat&FitStat ::operator = (FitStat & other) {
  chiSquare = other.chiSquare;
  df = other.df;
  num = other.num;
  return *this;
}

void FitStat::apply(double deviation){
  ++num;
  chiSquare += squared(deviation);
}
