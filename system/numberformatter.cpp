#include "numberformatter.h"

#include "charformatter.h"


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

Text NumberFormatter::operator ()(double value, bool addone) const {
  return format(value,addone);
}

Text NumberFormatter::makeNumber(double value){
  NumberFormatter formatter(17);
  return formatter(value,false);
}
