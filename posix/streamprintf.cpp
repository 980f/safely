#include "streamprintf.h"

//using namespace std;

#include "char.h"
//#include "index.h"
#include "cheaptricks.h"

void StreamFormatter::beginParse(){
  pushed.record(cout);
  dropIndex();
  dropFormat();
}

void StreamFormatter::dropIndex(){
  parsingIndex = false;
  argIndex = BadIndex;
}

void StreamFormatter::clearFormatValue(){
  formatValue = BadIndex;
  invertOption = false;
}

void StreamFormatter::applyFormat(StreamFormatter::FormatValue whichly){
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

void StreamFormatter::dropFormat(){
  parsingFormat = false;
  keepFormat = false;
  clearFormatValue();
}

void StreamFormatter::startFormat(){
  parsingIndex = false;
  parsingFormat = true;
}

bool StreamFormatter::appliedDigit(char c, unsigned &accumulator){
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


void StreamFormatter::startIndex(){
  parsingIndex = true;
  argIndex = BadIndex;
}

StreamFormatter::Action StreamFormatter::applyItem(char c){
  if(flagged(slashed)){
    if(parsingFormat) {
      cout.fill(c);
      return FeedMe;
    } else if(parsingIndex){
      startFormat();//and fall through
    } else {
      return Pass;
    }
  }
  if(parsingIndex) {
    if(appliedDigit(c,argIndex)) {
      return FeedMe;
    }
    if(c=='}') {
      return DoItem;
    }
    if(c==':') { //definitive start of format spec
      startFormat();
      return FeedMe;
    }
    startFormat();//nominally illegal, treat like 1st char of format spec.
    //and fall through to if(parsingFormat)
  }
  if(parsingFormat) {
    if(appliedDigit(c,formatValue)) {
      return FeedMe;
    }
    switch(c) {
    case '}':          //not sure what we should do with formatValue, some default thing like width?
      applyFormat(Widthly);
      return DoItem;
    case '!':
      //todo: do NOT reset stream after print
      keepFormat = true;
      break;
    case '-':
      invertOption = true;
      break;
    case 'w':
      applyFormat(Widthly);
      break;
    case 'p':
      applyFormat(Precisely);
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
    default:
      //use 'c' as fill char:
      cout.fill(c);
      break;
    }     // switch
    clearFormatValue();//just the value, not the state info
    return FeedMe;
  }
  if(c=='{') {
    startIndex();
    return FeedMe;
  }
  return Pass;
} // StreamPrintf::printNow

void StreamFormatter::afterActing(){
  if(!keepFormat) {
    pushed.restore(cout);
  }
  dropIndex();
  dropFormat();
}

StreamFormatter::StreamFormatter(std::ostream &ostr) : cout(ostr){
  beginParse();//sets all other fields.
  //theoretically we don't need to do the above, but it helps with debug to not get distracted with lingering trash.
}

void StreamFormatter::StreamState::record(std::ostream &cout){
  flags = cout.flags();
  width = cout.width();
  precision = cout.precision();
  fill = cout.fill();
}

void StreamFormatter::StreamState::restore(std::ostream &cout){
  cout.flags(flags);
  cout.width(width);
  cout.precision(precision);
  cout.fill(fill);
}

///////////////////
void StreamPrintf::printMissingItem(){
  write('{');
  write(argIndex);  //but formatting is long lost, this is an indicator of where the problem is so this is good enough.
  write('}');
}

StreamPrintf::StreamPrintf(std::ostream &cout):StreamFormatter (cout){}
