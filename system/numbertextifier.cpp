//"(C) Andrew L. Heilveil, 2017"
#include "numbertextifier.h"

#include "charformatter.h"

using namespace Safely;

NumberTextifier::NumberTextifier(int precision, TextKey postfix) :
  postfix(postfix){
  nf.decimals = precision;
}

unsigned NumberTextifier::needs(double value) const {
  return nf.needs(value) + postfix.length();
}

Text NumberTextifier::format(double value,bool addone) const {
  char widest[50];//Zguard(needs(value)+addone)];
  CharFormatter workspace(widest,sizeof(widest));
  workspace.zguard();//protect our null
  if(workspace.printNumber(value,nf,addone)) {//testing so that we don't add units if number failed to print
    workspace.printString(postfix);
  }
  workspace.next()=0;
  return Text(workspace.internalBuffer());//the constructor invoked here copies the content.
}

Text NumberTextifier::operator ()(double value, bool addone) const {
  return format(value,addone);
}

Text NumberTextifier::makeNumber(double value){
  NumberTextifier formatter(17);
  return formatter(value,false);
}
