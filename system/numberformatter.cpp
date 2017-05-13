#include "numberformatter.h"

#include "charformatter.h"


NumberFormatter::NumberFormatter(int precision, TextKey postfix) :
  postfix(postfix){
  nf.decimals = precision;
}

unsigned NumberFormatter::needs(double value) const {
  return nf.needs(value) + postfix.length();
}

Text NumberFormatter::format(double value,bool addone) const {
  char widest[Zguard(needs(value)+addone)];
  CharFormatter workspace(widest,sizeof(widest));
  workspace.zguard();//protect our null
  if(workspace.printNumber(value,nf,addone)) {//testing so that we don't add units if number failed to print
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
