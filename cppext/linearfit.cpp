#include "linearfit.h"


Linear LinearFit::equation(){//#Linear's are cheap enough to copy, hence no '&' in the return.
  Linear fn;
  fn[1]=slope();
  fn[0]=yy.mean()-xx.mean()*fn[1];
  return fn;
}

bool LinearFit::isMeaningful(bool barely) const {
  int num=N();
  return num>2||(barely&&num==2);
}


LinearFit::LinearFit(){

}
