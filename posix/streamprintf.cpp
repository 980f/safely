#include "streamprintf.h"

//using namespace std;

#include "char.h"
//#include "index.h"

void StreamPrintf::beginParse(){
  pushedFlags = cout.flags();
  dropIndex();
  dropFormat();
}

void StreamPrintf::dropIndex(){
  parsingIndex = false;
  argIndex = BadIndex;
}

void StreamPrintf::clearFormatValue(){
  formatValue = BadIndex;
  invertOption = false;
}

void StreamPrintf::applyFormat(StreamPrintf::FormatValue whichly){
  if(isValid(formatValue)) {
    switch(whichly) {
    case Widthly:
      cout.width(formatValue);
      break;
    case Precisely:
      cout.precision(formatValue);
      break;
    }
  }
  clearFormatValue();

} // StreamPrintf::applyFormat

void StreamPrintf::dropFormat(){
  parsingFormat = false;
  keepFormat = false;
  clearFormatValue();
}

void StreamPrintf::startFormat(){
  parsingIndex = false;
  parsingFormat = true;
}

bool StreamPrintf::appliedDigit(char c, unsigned &accumulator){
  if(Char(c).isDigit()) {
    unsigned digit = c - '0';
    if(isValid(accumulator)) {
      accumulator *= 10;
      accumulator += digit;
    } else {
      //is first digit
      accumulator = digit;
    }
    return true;
  } else {
    return false;
  }
} // StreamPrintf::appliedDigit

void StreamPrintf::missingField(){
  write('{');
  write(argIndex);  //but formatting is long lost, this is an indicator of where the problem is so this is good enough.
  write('}');
}

void StreamPrintf::startIndex(){
  parsingIndex = true;
  argIndex = BadIndex;
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
    missingField();//emits just the index, leaves off the formatting content, which was already acted upon!
    afterPrinting();
    return false;
  }
  if(parsingFormat) {
    if(appliedDigit(c,formatValue)) {
      return false;
    }
    switch(c) {
    case '}':          //not sure what we should do with formatValue, some default thing like width?
      applyFormat(Widthly);
      return true;
    case '!':
      //todo: do NOT reset stream after print
      keepFormat = true;
      break;
    case '-':
      invertOption = true;
      break;
    case 'w':
      applyFormat(Widthly);
      return false;
    case 'p':
      applyFormat(Precisely);
      return false;
    default:
      //use 'c' as fill char:
      cout.fill(c);
      break;
    case 'l':          //left align
      applyFormat(Widthly);
      cout.setf(std::ios_base::left);
      break;
    case 'r':          //right align
      applyFormat(Widthly);
      cout.setf(std::ios_base::right);
      break;
    case 'i':          //internal align, fills between sign and digits, 0x and hexdigits, money sign and value.
      applyFormat(Widthly);
      cout.setf(std::ios_base::internal);
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
      cout.setf(std::ios_base::scientific);
      break;
    case 'f':          //float
      cout.setf(std::ios_base::fixed);
      break;
    case 'x':
      cout.setf(std::ios_base::showbase);
      break;
    }     // switch
    clearFormatValue();//just the value, not the state info
    return false;
  }
  if(c=='{') {
    startIndex();
    return false;
  }
  write(c);
  return false;
} // StreamPrintf::printNow

void StreamPrintf::afterPrinting(){
  if(!keepFormat) {
    cout.flags(pushedFlags);//pop
  }
  dropIndex();
  dropFormat();
}

StreamPrintf::StreamPrintf(std::ostream &ostr) : cout(ostr){
  beginParse();//sets all other fields.
  //theoretically we don't need to do the above, but it helps with debug to not get distracted with lingering trash.
}
