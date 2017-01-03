#include "numberparser.h"

#include "minimath.h"
#include "cheaptricks.h"

#include "ctype.h"

/** maximum value that can be multiplied by 10 and not exceed 2^64: */
const u64 DecimalCutoff = (1UL<<63)/5; //2^64 /10 == 2^63/5, needed to take care that the compiler didn't get a wrap.

double NumberParserPieces::packed() const {
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

bool NumberParserPieces::seemsOk() const {
  if (!isNan&&!isInf){
    if(isZero || predecimal || postdecimal){//perfect zero or had some digits
      return true;
    }
  }
  return false;
} // NumberParserState::packed

bool NumberParserPieces::startsNumber(char c){
  return isdigit(c) || c == '-' || c == '.'; //'.' tolerates lack of a leading zero
}

void NumberParserPieces::reset(void){
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


void PushedNumberParser::reset(){
  NumberParserPieces::reset();
  processed = 0;
  phase=Start;
}

bool PushedNumberParser::applydigit(u64 &accum,char ch){
  //todo: check for accumulator overflow. If so then we don't shift in digits, just inc pow10 as relevent.
  if(accum > DecimalCutoff){
    return false;
  }
  unsigned digit=unsigned(ch-'0');
  //todo: base 36 logic.
  accum*=base;
  accum+=digit;
  return true;
}

bool PushedNumberParser::isExponentMarker(char ch){
  return (ch|0x20)=='e';//fast tolower
}

bool PushedNumberParser::fail(){
  if(changed(phase,Failed)){
   //a place to breakpoint
    return false;
  }
  return false;
}


bool PushedNumberParser::next(char u){
  ++processed;//includes char that causes termination.
  switch(phase){
  default:
    return fail();//if you push at me when I'm done I will bitch back at you.
  case Start:
    switch (u) {
    case '-':
      if(changed(negative,true)){//first negative ok
        return true;
      } else {//additional negative not cool
        return fail();
      }
    case '+':
      return true;
    case '.'://tolerate leading point
      phase=AfterDecimal;
      return true;
    case '0'://must exclude from 'isdigit'
      isZero=true;
      phase=BeforeDecimal;
      return true;
    default:
      if(isdigit(u)){
        //first digit of predecimal
        phase=BeforeDecimal;
        applydigit(predecimal,u);
        return true;
      } else if(isspace(u)){
        return true;
      } else {
        return fail();//if you push at me when I'm done I will bitch back at you.
      }
    }
//    break;
  case BeforeDecimal: //only digits and decimal point
    if(isdigit(u)){
      if(isZero){//then started with a zero
        //this is where we would detect a base xX
        return fail();//no bases other than 10
      }
      if(applydigit(predecimal,u)){
        //copacetic
      } else {//count dropped digits
        if(!pow10){
          if(u>='5'){
            ++predecimal;//round missing stuff
          }
        }
        ++pow10; //if pow10 wraps we have a massively long string of digits.
      }
      return true;
    } else if('.'==u){
      if(isZero){//checking for debug
        isZero=false;
      }
      hadRadixPoint=true;
      phase=AfterDecimal;
      return true;
    } else if(isExponentMarker(u)){
      if(isZero){ //0E not allowed
        return fail();
      }
      phase=AfterExponent;
      return true;
    } else {
      phase=Done;
      return false;
    }

  case AfterDecimal:
    if(isdigit(u)){
      if(pow10==0){//if we dropped digits then we ignore the AfterDecimal content
        if (applydigit(postdecimal,u)){
          ++div10;
        }
      }
      return true;
    } else if(isExponentMarker(u)){
      phase=AfterExponent;
      return true;
    } else {
      phase=Done;
      return false;
    }

  case AfterExponent: //meaning after the 'E'
    if(isdigit(u)){
      hasEterm=true;//not set until we have at least one digit
      applydigit(exponent,u);
      return true;
    } else if('-'==(u)){
      negativeExponent=true;
      return true;
    } else {
      if(exponent){
        phase=Done;
        return false;
      } else {
        return fail();
      }
    }
  }
}

int PushedNumberParser::ilog10() const{
  int totalPow10=::ilog10(predecimal);
  totalPow10+=this->pow10;
  if(hasEterm){
    if(negativeExponent){
      totalPow10-=exponent;
    } else {
      totalPow10+=exponent;
    }
  }
  return totalPow10;
}

double PushedNumberParser::value(){
  lastParsed=packed();
  return lastParsed;
}

