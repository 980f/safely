#include "numberformatter.h"

#include "charformatter.h"

NumberFormatter::NumberFormatter(bool fp, int precision, TextKey postfix) :
  fp(fp),
  precision(precision),
  postfix(postfix){
  //#nada
}

Text NumberFormatter::format(double value,bool addone) const {
  char widest[17+1+1+precision+postfix.length()];//gross overestimate of maximum number as text
  CharFormatter workspace(widest,sizeof (widest));
  if(fp){
    if(workspace.printNumber(value,precision+addone)){//don't add units if number failed to print
      workspace.printString(postfix);
    }
  } else {
//    if(workspace.printSigned(int(value),precision+addone)){//don't add units if number failed to print
//      workspace.printString(postfix);
//    }
  }
  return Text(workspace.internalBuffer());
}
