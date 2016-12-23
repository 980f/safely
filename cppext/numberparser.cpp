#include "numberparser.h"

//#include "ctype.h"

bool isDigit(char c){
  return c>='0'&&c<='9';
}

const u64 DecimalCutoff = 922337203685477580LL; //trunc(2^63)/10, courtesy of python.

double NumberParserState::packed() const {
  if(isNan) {
    return Nan;
  }
  if(isInf) {
    return negative ? -Infinity : Infinity;
  }
  int exp = exponent;
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
} // NumberParserState::packed

bool NumberParserState::startsNumber(char c){
  return isDigit(c) || c == '-' || c == '.'; //'.' tolerates lack of a leading zero
}

void NumberParserState::reset(void){
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

double NumberParser::getValue(LatentSequence<char>&p){
  if(parseNumber(p)) {
    return packed();
  } else {
    return Nan;
  }
} /* getValue */

int NumberParser::parseUnsigned(u64&n, PeekableSequence<char>&p){
  int processed = 0;
  int skipped = 0;
  u64 acc = 0; //need to accept 53 bits worth for mantissa of double.
  u8 ch = 0; //init for debug

  while(p.hasNext()) {
    ch = p.peek();
    if(isDigit(ch)) { //todo:?M purge leading zeroes
      p.skip(1);
      ++processed;
      if(acc > DecimalCutoff) {
        ++skipped;
      } else {
        acc *= 10;
        acc += ch - '0';
      }
    } else {
      break;
    }
  }
  if(processed > 0) {
    n = acc;
    return skipped;
  } else {
    return -1; //signal
  }
} /* parseUnsigned */

void NumberParser::parseFraction(PeekableSequence<char>&p){
  u64 acc = 0; //need to accept 53 bits worth for mantissa of double.
  u8 ch = 0; //init for debug
  div10 = 0;
  while(p.hasNext()) {
    ch = p.peek();
    if(isDigit(ch)) {
      p.skip(1);
      if(acc > DecimalCutoff) {
        continue;//simply ignore excess precision.
      } else { //leading zeroes come here, for a minor amount of wasted effort.
        acc *= 10;
        acc += ch - '0';
        ++div10;
      }
    } else {
      break;
    }
  }
  postdecimal = acc;
  //  return div10; //signal
} // NumberParser::parseFraction

bool NumberParser::parseNumber(LatentSequence<char>&p){
  reset();
  u8 ch = 0;
  while(p.hasNext()) { //accept leading whitespace
    ch = p.next();
    if(ch != ' ') { //todo: used to tolerate tabs and newlines
      break;
    }
  }
  if(ch == '-') { //optional minus sign
    negative = true;
    if(!p.hasNext()) {
      return false; //true results in a value of zero... negative sign by itself is not a number
    }
  } else if(ch == '+') { //optional '+' sign
    if(!p.hasNext()) {
      return false; //true results in a value of zero... positive sign by itself is not a number
    }
    //else just move along
  } else if(isDigit(ch) || '.') { //don't tolerate an additional '-'
    p.unget(); //put the digit back
  } else {
    return false; //not a decimal string
  }
  pow10 = parseUnsigned(predecimal, p);
  if(pow10 >= 0) { //negative is signal, no number found.
    //still have to parse for E expression if string is 0Exx
    if(p.hasNext()) {
      ch = p.peek();
      if(ch == '.') {
        p.skip(1);
        parseFraction(p);
      }
      if(p.hasNext()) {
        ch = p.peek();
        //space before the E is not tolerated,
        if(ch == 'E' || ch == 'e') { //maydo: also tolerate a 'D'
          hasEterm = true;
          p.skip(1);
          if(p.hasNext()) {
            ch = p.peek(); //char after the E
          }
          switch(int(ch)) {
          case '-':
            negativeExponent = true;
            p.skip(1);
            break;
          case '+':
            p.skip(1);
            break;
          }
          //space after the E is not tolerated
          int overflow = parseUnsigned(exponent, p);
          if(overflow < 0) {
            return false; //must have digits after E or E- or E+
          }
          if(overflow > 0) { //unbelievably large exponent
            return false;
          }
        }//process E
      }//check for an E
    }//check for .99 or Exxx
    return true;
  } else {
    //whitespace between sign and number is not tolerated
    return false;
  }
} /* parseNumber */

void PushedNumberParser::reset(){
  NumberParserState::reset();
  processed = 0;
  skipped = 0;
  ch = 0; //init for debug
}
