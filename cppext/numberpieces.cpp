#include "numberpieces.h"

#include "math.h"  //must precede minimath for intbin to have access to modf
#include "minimath.h"
#include "cheaptricks.h"
#include "char.h"


template <>double intbin<double,double>(double&);

double NumberPieces::packed() const {
  if(isNan) {
    return Nan;
  }
  if(isInf) {
    return negative ? -Infinity : Infinity;
  }
  int exp = int(exponent);//safe truncation
  if(negativeExponent) {
    exp = -exp;
  }
  //exp is now the user given exponent

  double number = predecimal;
  if(pow10 > 0) { //then trailing digits of predecimal part were lopped off
    number *= ::pow10(pow10);
    //ignore all postdecimal processing as numerically insignificant.
  } else {
    //have to figure out how many digits the fractional part had
    double fract = postdecimal;
    fract *= ::pow10(-div10);
    number += fract;
  }
  number *= ::pow10(exp);//and apply user provide power
  return negative ? -number : number;
}

NumberPieces::NumberPieces(double d){
  reset();
  decompose(d);
}

bool NumberPieces::seemsOk() const {
  if (!isNan&&!isInf){
    if(isZero || predecimal || postdecimal){//perfect zero or had some digits
      return true;
    }
  }
  return false;
} // NumberParserState::packed

bool NumberPieces::startsNumber(char c){
  return Char(c).isDigit() || c == '-' || c == '.'; //'.' tolerates lack of a leading zero
}

void NumberPieces::reset(void){
  isNan = false;
  isInf = false;
  negative = false;
  predecimal = 0;
  pow10 = 0;
  postdecimal = 0;
  div10 = 0;
  hasEterm = false;
  exponent = 0;
  negativeExponent = false;
} // NumberParserState::reset


void NumberPieces::decompose(double d){
  reset();
  if(::isNan(d)){
    isNan=true;
    return;
  }
  negative=signabs(d);
  if(isSignal(d)){
    isInf=true;
    return;
  }

  if(isNormal(d)){
    u64 whole=intbin<u64,double>(d);
    pow10= ilog10(whole);
    if(pow10<0){//number less than 1
      div10=-pow10;
      postdecimal=d * ::pow10(div10);
      pow10=0;//jic
      return;
    }
    if(pow10>19 /*(floor(log10(2^64)*/){
      //divide by 10 until it fits
      pow10-=19;//is supposed to be extra zeroes needed.
      whole*=::pow10(pow10);
      predecimal=unsigned(whole);
    }
  } else {
    //todo:wtf?
  }
}
