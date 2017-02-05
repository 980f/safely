#include "streamprintf.h"

//using namespace std;

#include "char.h"
#include "cheaptricks.h"

void StreamPrintf::dropIndex(){
  parsingIndex = false;
  argIndex = ~0;
}

void StreamPrintf::dropFormat(){
  parsingFormat = false;
  formatValue = ~0;
  invertOption = false;
  keepFormat = false;
}

void StreamPrintf::startFormat(){
  parsingIndex = false;

  parsingFormat = true;
  formatValue = 0;

  invertOption = false;
  keepFormat = false;
}

bool StreamPrintf::appliedDigit(char c, unsigned &accumulator){
  if(Char(c).isDigit()) {
    accumulator *= 10;
    accumulator += c - '0';    //primitive get digit
    return true;
  } else {
    return false;
  }
}

void StreamPrintf::missingField(){
  write('{');
  write(argIndex);  //but formatting is long lost, this is an indicator of where the problem is so this is good enough.
  write('}');
}

void StreamPrintf::startIndex()
{
  parsingIndex = true;
  argIndex = 0;
}

bool StreamPrintf::printNow(char c){
  if(parsingIndex) {
    if(appliedDigit(c,argIndex)) {
      return false;
    }
    if(c==':') {    //format tweak
      startFormat();
      return false;
    }
    if(c=='}') {
      return true;
    }
    //else is bad format string, what do we do?
    missingField();
    dropIndex();
    //but proceed with trash char getting printed
    return false;
  }
  if(parsingFormat) {
    if(appliedDigit(c,formatValue)) {
      return false;
    }
    switch(c) {
    case '}':          //not sure what we should do with formatValue, some default thing like width?
      if(formatValue) {
        cout.width(formatValue);
      }
      return true;
    case '!':
      //todo: do NOT reset stream after print
      keepFormat = true;
      break;
    case '-':
      invertOption = true;
      break;
    case 'w':
      cout.width(formatValue);
      break;
    case 'p':
      cout.precision(formatValue);
      break;
    default:
      //use 'c' as fill char:
      cout.fill(c);
      break;
    case 'l':          //left align
      break;
    case 'r':          //fight align
      break;
    case 'i':          //internal align
      break;
    case 'd':          //decimal
      cout << std::dec;
      break;
    case 'o':          //octal
      cout << std::oct;
      break;
    case 'b':          //binary
    case 'h':          //hex
      cout << std::hex;
      break;
    case 's':          //scientific
    case 'f':          //float
      //todo: showbase (0x prefix and the like)
      break;
    }     // switch
    formatValue = 0;
    invertOption = 0;
    return false;
  }
  if(c=='{') {
    startIndex();
    return false;
  }
  write(c);
  return false;
} // StreamPrintf::printNow

StreamPrintf::StreamPrintf(std::ostream &ostr) : cout(ostr){
}
