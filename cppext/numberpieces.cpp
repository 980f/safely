//"(C) Andrew L. Heilveil, 2017"
#include "numberpieces.h"

#include "math.h"  //must precede minimath for intbin to have access to modf
#include "minimath.h"
#include "cheaptricks.h"
#include "char.h"

static const unsigned maxDigits=19; /*(floor(log10(2^64)*/
static const double p19=::pow10(19);

template <>double intbin<double,double>(double&);

double NumberPieces::packed() const {
  if(isNan) {
    return Nan;
  }
  if(isInf) {
    return negative ? -Infinity : Infinity;
  }
  if(isZero){
    return 0.0;
  }

  double number = predecimal;
  if(pow10 > 0) { //then trailing digits of predecimal part were lopped off
    number *= ::pow10(pow10);
    //ignore all postdecimal processing as numerically insignificant.
  } else {
    //have to figure out how many digits the fractional part had
    double fract = postdecimal;
    fract /= p19;
    number += fract;
  }
  if(hasEterm){
    //note that exponent sometimes is used as a cache for printing logic, it is not to be applied unless hasEterm is true.
    int exp = int(exponent);//safe truncation
    if(negativeExponent) {
      exp = -exp;
    }
//      exp is now the scientific notation exponent
    number *= ::pow10(exp);//and apply user provide power
  }
  return negative ? -number : number;
}

NumberPieces::NumberPieces(double d){
//  reset();
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
  isZero = false;
  negative = false; hadRadixPoint=false;
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

  negative=signabs(d)<0;
  if(isSignal(d)){
    isInf=true;
    return;
  }
  if(d==0.0){
    isZero=true;
    return;
  }
  if(isNormal(d)){
    double exp=log10(d);
    negativeExponent=signabs(exp)<0;
    exponent=int(exp);

    //todo:0 check against DecimalCutoff, intbin will truncate if d>than that.
    double fraction=d;
    predecimal=intbin<u64,double>(fraction);
    postdecimal=fraction * p19;//as many digits as we dare

//    if(negativeExponent){//number less than 1
////todo: try to grab some extra digits      div10=exponent;
////      postdecimal=fraction*::pow10(div10) * p19;
//    } else
    if(exponent>maxDigits){
      //divide by 10 until it fits
      pow10=exponent-maxDigits;
      fraction=d * ::pow10(-pow10);
      predecimal=intbin<u64,double>(fraction);
      hasEterm=true;
    }
  } else {
    //todo:wtf do we do with deNorms?
  }
}
