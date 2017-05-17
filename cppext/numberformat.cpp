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
  if(isValid(fieldWidth)&&fieldWidth>0) {
    return fieldWidth;
  } else {
    return decimals>=0 ? 19 + 1 + 1 + decimals : 3-decimals;
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
  if(scientific){
    //sigfigs+1 if sigfigs>exponent sa we'll need a d.p.
    //exponent if >sigfigs then add pow10 for its extra zeroes
  } else {
    //leading zero if number <0, else add room
    necessary+=decimals+decimals>0;
    if(preprint->negativeExponent){
      ++necessary;
    } else {
      necessary+=(1+preprint->exponent);
    }
  }
  //we have satisfied number of decimals or sigfigs requirement.
  if(preprint->negative||showsign) {//print optional sign or forced sign
    ++necessary;
  }

  if(isValid(fieldWidth)){
    if(necessary<=fieldWidth){
      //will print some blanks and proceed, we only implement right-align in fixed fields.
      return fieldWidth;
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
  decimals = 0;
  scientific=false;
  showsign=false;
}

void NumberFormat::onUse(){
  if(usages.last()) {
    clear();//only clear on the transition to not used, if count already 0 then use forever.
  }
}
