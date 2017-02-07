#include "streamformatter.h"


#include "char.h"
#include "index.h"
#include "cheaptricks.h"

void StreamFormatter::beginParse(){
  pushed.record(stream);
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
      stream.width(formatValue);
      break;
    case Precisely:
      stream.precision(formatValue);
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
  Char digital(c);
  if(digital.isDigit()) {
    if(!isValid(accumulator)) {
      accumulator=0;
    }
    digital.applyTo(accumulator);
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
      stream.fill(c);
      return FeedMe;
    } else if(parsingIndex){
      startFormat();//and fall through
    } else {
      return Escaped;
    }
  }
  if(c=='\\'){
    slashed=true;
    return FeedMe;
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
      stream.setf(std::ios_base::left);
      break;
    case 'r':          //right align
      applyFormat(Widthly);
      stream.setf(std::ios_base::right);
      break;
    case 'i':          //internal align, fills between sign and digits, 0x and hexdigits, money sign and value.
      applyFormat(Widthly);
      stream.setf(std::ios_base::internal);
      break;
    case 'd':          //decimal
      stream.setf(std::ios_base::dec);
      break;
    case 'o':          //octal
      stream.setf(std::ios_base::oct);
      break;
    case 'b':          //binary
    case 'h':          //hex
      stream.setf(std::ios_base::hex);
      break;
    case 's':          //scientific
      stream.setf(std::ios_base::scientific);
      break;
    case 'f':          //float
      stream.setf(std::ios_base::fixed);
      break;
    case 'x':
      stream.setf(std::ios_base::showbase);
      break;
    default:
      //use 'c' as fill char:
      stream.fill(c);
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
    pushed.restore(stream);
  }
  dropIndex();
  dropFormat();
}

StreamFormatter::StreamFormatter(std::ios &ostr) : stream(ostr){
  beginParse();//sets all other fields.
  //theoretically we don't need to do the above, but it helps with debug to not get distracted with lingering trash.
}

void StreamFormatter::StreamState::record(std::ios &cout){
  flags = cout.flags();
  width = cout.width();
  precision = cout.precision();
  fill = cout.fill();
}

void StreamFormatter::StreamState::restore(std::ios &stream){
  stream.flags(flags);
  stream.width(width);
  stream.precision(precision);
  stream.fill(fill);
}
