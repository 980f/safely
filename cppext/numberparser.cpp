//"(C) Andrew L. Heilveil, 2017"
#include "numberparser.h"

#include "minimath.h"
#include "cheaptricks.h"
#include <cctype>

void PushedNumberParser::reset() {
  NumberPieces::reset();
  processed = 0;
  phase = Start;
  lastParsed = Nan; //don't reset until after you have saved this away.
}

bool PushedNumberParser::applydigit(u64 &accum, char ch) {
  //check for accumulator overflow. If so then we don't shift in digits
  if (accum > DecimalCutoff) {
    return false;
  }
  auto digit = unsigned(ch - '0');
  if (digit > base) { //added this check to make this function useful outside of this class.
    return false;
  }
  accum *= base;
  accum += digit;
  return true;
}

bool PushedNumberParser::isExponentMarker(char ch) {
  return (ch | 0x20) == 'e'; //fast tolower
}

bool PushedNumberParser::fail() {
  if (changed(phase, Failed)) {
    return false; //#leave expanded, this is a place to breakpoint
  }
  return false;
}


bool PushedNumberParser::next(char u) {
  ++processed; //includes char that causes termination.
  switch (phase) {
    default:
      return fail(); //if you push at me when I'm done I will bitch back at you.
    case Start:
      switch (u) {
        case '-':
          if (changed(negative, true)) { //first negative ok
            return true;
          } else { //additional negative not cool
            return fail();
          }
        case '+':
          if (negative) {
            return fail(); //only one sign indicator is allowed,
          }
          return true;
        case '.': //tolerate leading point
          phase = AfterDecimal;
          return true;
        case '0': //must exclude from 'isdigit'
          isZero = true;
          phase = BeforeDecimal;
          return true;
        default:
          if (isdigit(u)) {
            //first digit of predecimal
            phase = BeforeDecimal;
            applydigit(predecimal, u);
            //       if(applydigit(predecimal,u)){
            //         ++preDigits;
            //       }
            return true;
          }
          if (isspace(u)) {
            return true; //leading spaces is OK
          }
          return fail(); //unacceptible start character
      }
    //#all paths in case have a return.
    case BeforeDecimal: //only digits and decimal point
      if (isdigit(u)) {
        if (isZero) { //then started with a zero
          //this is where we would detect a base xX
          return fail(); //no bases other than 10
        }
        if (pow10) {//then we have run out of range for more digits
          ++pow10;
        } else if (applydigit(predecimal, u)) {//copacetic
          //++preDigits;
        } else { //start counting dropped digits
          if (!pow10) {
            if (u >= '5') {//"round away from zero"
              ++predecimal; //round missing stuff
            }
          }
          ++pow10; //if pow10 wraps we have a massively long string of digits.
        }
        return true;
      }
      if ('.' == u) {
        if (isZero) { //checking for debug
          isZero = false;//this line is only reached for [-]0.*
        }
        hadRadixPoint = true;
        phase = AfterDecimal;
        return true;
      }
      if (isExponentMarker(u)) {
        if (isZero) { //0E not allowed
          return fail();
        }
        phase = AfterExponent;
        return true;
      }
      phase = Done;
      return false;//FYI: value is an integer.
    case AfterDecimal:
      if (isdigit(u)) {
        if (pow10 == 0) { //if we dropped digits then we ignore the AfterDecimal content
          if (applydigit(postdecimal, u)) {
            ++postDigits;
          }
        }
        return true;
      }
      if (isExponentMarker(u)) {
        phase = AfterExponent;
        return true;
      }
      phase = Done;
      return false;
    case AfterExponent: //meaning after the 'E'
      if (isdigit(u)) {
        hasEterm = true; //not set until we have at least one digit
        applydigit(exponent, u);
        return true;
      }
      if ('-' == u) {
        if (changed(negativeExponent, true)) {
          return true;
        }
        return fail();//E-- not allowed
      }
      if (exponent) {
        phase = Done;
        return false;
      }
      return fail();
  }//switch phase
}

int PushedNumberParser::ilog10() const {
  int totalPow10 = ::ilog10(predecimal);
  totalPow10 += this->pow10;
  if (hasEterm) {
    if (negativeExponent) {
      totalPow10 -= exponent;
    } else {
      totalPow10 += exponent;
    }
  }
  return totalPow10;
}

double PushedNumberParser::value() {
  lastParsed = packed();
  return lastParsed;
}
