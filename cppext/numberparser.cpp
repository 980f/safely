//"(C) Andrew L. Heilveil, 2017"
#include "numberparser.h"

#include "minimath.h"
#include "cheaptricks.h"
#include "ctype.h"

void PushedNumberParser::reset(){
  NumberPieces::reset();
  processed = 0;
  phase=Start;
}

bool PushedNumberParser::applydigit(u64 &accum,char ch){
  //check for accumulator overflow. If so then we don't shift in digits, just inc pow10 as relevent.
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

