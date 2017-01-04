#include "numberformatter.h"

#include "charformatter.h"

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

void NumberFormat::clear(){
  fieldWidth = BadLength;
  precision = 17; //ieee 64bit
}

void NumberFormat::onUse(){
  if(usages && !--usages) {
    clear();
  }
}

NumberFormatter::NumberFormatter(int precision, TextKey postfix) :
  postfix(postfix){
  nf.precision = precision;
}

unsigned NumberFormatter::needs() const {
  return nf.needs() + postfix.length();
}

Text NumberFormatter::format(double value,bool addone) const {
  char widest[Zguard(needs())];//gross overestimate of maximum number as text
  CharFormatter workspace(widest,sizeof(widest));

  if(workspace.printNumber(value,nf.precision + addone)) {//don't add units if number failed to print
    workspace.printString(postfix);
  }

  return Text(workspace.internalBuffer());//the constructor invoked here copies the content.
}
