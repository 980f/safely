//"(C) Andrew L. Heilveil, 2017"
#include "numberformat.h"

#include <minimath.h>

#include "index.h" //isValid(unsigned)

NumberFormat::NumberFormat() {
  clear();
}

unsigned NumberFormat::needs() const { //todo:0 very bogus math herein
  if (isValid(fieldWidth) && fieldWidth > 0) {
    return fieldWidth;
  }
  return decimals >= 0 ? 19 + 1 + 1 + decimals : 3 - decimals;
}

//this code must track charformatter::printnumber
unsigned NumberFormat::needs(double value, NumberPieces *preprint) const {
  NumberPieces p;
  if (preprint == nullptr) {
    preprint = &p;
  }
  preprint->decompose(value);
  if (preprint->isZero) {
    return 1;
  }
  if (preprint->isNan) {
    return 4; //size of our text
  }
  if (preprint->isInf) {
    return 4; //we need a sign in various usages to even call this guy.
  }
  unsigned necessary = 0;
  if (scientific) {
    //sigfigs+1 if sigfigs>exponent we'll need a d.p.
    //exponent if >sigfigs then add pow10 for its extra zeroes
    necessary += 10; //todo:00 finish this!
  } else {
    //leading zero if number <0, else add room
    necessary += decimals + (decimals > 0); //todo:00 suspicious for negative decimals.
    if (preprint->negativeExponent) {
      ++necessary;
    } else {
      necessary += (1 + preprint->exponent);
    }
  }
  //we have satisfied number of decimals or sigfigs requirement.
  if (preprint->negative || showsign) { //print optional sign or forced sign
    ++necessary;
  }

  if (isValid(fieldWidth)) {
    if (necessary <= fieldWidth) {
      //will print some blanks and proceed, we only implement right-align in fixed fields.
      return fieldWidth;
    }
    if (preprint->postDigits > 0 && (necessary - preprint->postDigits) < fieldWidth) {
      //truncate precision since we can
      unsigned trunc = necessary - fieldWidth;
      preprint->postdecimal /= i64pow10(trunc);
      preprint->postDigits -= trunc;
    }
  }
  return necessary;
}

void NumberFormat::clear() {
  fieldWidth = BadLength;
  decimals = 0;
  scientific = false;
  showsign = false;
}

void NumberFormat::onUse() {
  if (usages.last()) {
    clear(); //only clear on the transition to not used, if count already 0 then use forever.
  }
}
