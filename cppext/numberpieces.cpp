//"(C) Andrew L. Heilveil, 2017"
#include "numberpieces.h"

#include <cmath>  //must precede minimath for intbin to have access to modf
#include "minimath.h"
#include "char.h"

template<> double intbin<double, double>(double &);

double NumberPieces::packed() const {
  if (isNan) {
    return Nan;
  }
  if (isInf) {
    return negative ? -Infinity : Infinity;
  }
  if (isZero) {
    return 0.0;
  }

  double number = predecimal;
  if (pow10 > 0) { //then trailing digits of predecimal part were lopped off
    number *= dpow10(pow10);
    //ignore all postdecimal processing as numerically insignificant.
  } else { //if(postdecimal){//postDigits isn't zero under reasonable conditions so let's not bother testing it.
    double fract = postdecimal;
    fract /= dpow10(postDigits);
    number += fract;
  }
  if (hasEterm) {
    //note that exponent sometimes is used as a cache for printing logic, it is not to be applied unless hasEterm is true.
    int exp = int(exponent); //safe truncation
    if (negativeExponent) {
      exp = -exp;
    }
    //      exp is now the scientific notation exponent
    number *= dpow10(exp); //and apply user provide power
  }
  return negative ? -number : number;
}

NumberPieces::NumberPieces(double d) {
  //  reset();
  decompose(d);
}

bool NumberPieces::seemsOk() const {
  if (!isNan && !isInf) {
    if (isZero || predecimal || postdecimal) { //perfect zero or had some digits
      return true;
    }
  }
  return false;
} // NumberParserState::packed

bool NumberPieces::startsNumber(char c) {
  return Char(c).isDigit() || c == '-' || c == '.'; //'.' tolerates lack of a leading zero
}

void NumberPieces::reset() {
  isNan = false;
  isInf = false;
  isZero = false;
  negative = false;
  hadRadixPoint = false;
  predecimal = 0;;
  pow10 = 0;
  postdecimal = 0;
  postDigits = 0;
  hasEterm = false;
  exponent = 0;
  negativeExponent = false;
}

void NumberPieces::decompose(double d) {
  reset();
  if (::isNan(d)) {
    isNan = true;
    return;
  }

  negative = signabs(d) < 0;
  if (isSignal(d)) {
    isInf = true;
    return;
  }
  if (d == 0.0) {
    isZero = true;
    return;
  }
  if (isNormal(d)) {
    double exp = log10(d); //this is somewhat expensive,
    negativeExponent = signabs(exp) < 0;
    exponent = unsigned(exp);

    //todo:0 check against DecimalCutoff, intbin will truncate if d>than that.
    double fraction = d;
    predecimal = intbin<u64, double>(fraction);
    //preDigits= unsigned(1+ilog10(predecimal));//0=>-1=>0  1=>0=>1  9=>0=>1 10=>1=>2.
    postdecimal = u64(fraction * p19); //as many digits as we dare
    postDigits = maxDigits; //or is it 19?
    if (!negativeExponent && exponent > maxDigits) { //predecimal was truncated by intbin
      //divide by 10 until it fits
      pow10 = unsigned(exponent - maxDigits);
      fraction = d * dpow10(-pow10);
      predecimal = intbin<u64, double>(fraction);
      //preDigits=1+ilog10(predecimal);//we can compute this from the prior machinations.
      postdecimal = 0; //anything here is garbage
      postDigits = 0; //
      hasEterm = true;
    }
  } else {
    //todo:wtf do we do with deNorms?
  }
}
