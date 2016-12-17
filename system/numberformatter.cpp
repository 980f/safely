#include "numberformatter.h"

#include <iomanip> //for setprecision


NumberFormatter::NumberFormatter(bool fp, int precision, const Ustring &postfix) :
  fp(fp),
  precision(precision),
  postfix(postfix){
  //#nada
}

Ustring NumberFormatter::format(double value,bool addone) const {
//  if(fp) {
//    return Ustring::format(setprecision(precision+(addone?1:0)), value, " ", postfix);
//  } else {
//    return Ustring::format(fixed, setprecision(precision+(addone?1:0)), value, " ", postfix);
//  }
  return Ustring("NYI");
}
