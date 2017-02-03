#include "numberformat.h"
#include "index.h"

#include "minimath.h"
#include <cmath>

#include "numberpieces.h"

NumberFormat::NumberFormat(){
  clear();
}

unsigned NumberFormat::needs() const {
  //todo:0 very bogus math herein
  if(fieldWidth>0) {
    return fieldWidth;
  } else {
    return precision>0 ? 17 + 1 + 1 + precision : -precision; //todo: add space for E-xxxx
  }
}

unsigned NumberFormat::needs(double value, NumberPieces *preprint) const {
  NumberPieces p;
  if(preprint==nullptr){
    preprint=&p;
  }
  preprint->decompose(value);
  if(preprint->isNan) {
    return 4;//size of our text
  }
  if(preprint->isInf) {
    return 4;//we need a sign in various usages to even call this guy.
  }

  unsigned necessary=0;

  if(preprint->negative) {//print optional sign
    ++necessary;
  }

  necessary+=ilog10(preprint->predecimal);
  if(preprint->pow10==0){
    return necessary;
  }
  necessary+=preprint->pow10;

  if(isValid(fieldWidth)){
    if(necessary<=fieldWidth){
      //will print some blanks and proceed
    } else {
      if(preprint->div10>0 && (necessary-preprint->div10)<fieldWidth){
        //truncate precision since we can
        int trunc=necessary-fieldWidth;
        preprint->postdecimal/= pow10(trunc);
        preprint->div10-=trunc;
      }
    }
  }

  return necessary;
}

void NumberFormat::clear(){
  fieldWidth = BadLength;
  precision = 17; //ieee 64bit
}

void NumberFormat::onUse(){
  if(usages.last()) {
    clear();
  }
}
