//"(C) Andrew L. Heilveil, 2017"
#include "numberpieces.h"

#include "math.h"  //must precede minimath for intbin to have access to modf
#include "minimath.h"
#include "cheaptricks.h"
#include "char.h"

static const unsigned maxDigits=19; /*(floor(log10(2^64)*/
static const double p19=dpow10(maxDigits);

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
    number *= dpow10(pow10);
    //ignore all postdecimal processing as numerically insignificant.
  } else {
    double fract = postdecimal;
    fract /= dpow10(postDigits);
    number += fract;
  }
  if(hasEterm){
    //note that exponent sometimes is used as a cache for printing logic, it is not to be applied unless hasEterm is true.
    int exp = int(exponent);//safe truncation
    if(negativeExponent) {
      exp = -exp;
    }
//      exp is now the scientific notation exponent
    number *= dpow10(exp);//and apply user provide power
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
  postDigits = 0;
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
    exponent=unsigned(exp);

    //todo:0 check against DecimalCutoff, intbin will truncate if d>than that.
    double fraction=d;
    predecimal=intbin<u64,double>(fraction);
    postdecimal=u64(fraction * p19);//as many digits as we dare
    postDigits=19;
    if(!negativeExponent && exponent>maxDigits){//predecimal was truncated by intbin
      //divide by 10 until it fits
      pow10=unsigned(exponent-maxDigits);
      fraction=d * dpow10(-pow10);
      predecimal=intbin<u64,double>(fraction);
      postdecimal=0;//anything here is garbage
      postDigits=0;//
      hasEterm=true;
    }
  } else {
    //todo:0 wtf do we do with deNorms?
  }
}
