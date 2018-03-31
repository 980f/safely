#include "settable.h"

//#include "art.h"
#ifndef MAXARGS
#define MAXARGS 10
#endif

bool Settable::blockCheck(Settable &desired) const{
  return differs(desired);
}

bool Settable::differs(const Settable &other)const{
  ArgBlock<MAXARGS> args;
  ArgBlock<MAXARGS> args2;

  other.getParams(args);
  this->getParams(args2);
  return !args2.equals(args);
}

void Settable::copy(const Settable &other){
  if(!&other){//compiler generated an operator = with a null 'other'
    return;
  }
  ArgBlock<MAXARGS> args;
  other.getParams(args);
  args.freeze();
  this->setParams(args);
}

bool Settable::changed(const Settable &other){
  copy(other);
  return wasModified();
}

Settable::~Settable(){
  //#nada
}
