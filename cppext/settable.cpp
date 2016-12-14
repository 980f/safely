#include "settable.h"

bool Settable::blockCheck(Settable &desired){
  return differs(desired);
}

bool Settable::differs(const Settable &other)const{
  MessageArgs;
  other.getParams(args);
  MessageArgs2;

  this->getParams(args2);
  return !args2.equals(args);
}

void Settable::copy(const Settable &other){
  if(!&other){//compiler generated an operator = with a null 'other'
    return;
  }
  MessageArgs;
  other.getParams(args);
  args.freeze();
  this->setParams(args);
}

bool Settable::differed(const Settable &other){
  copy(other);
  return wasModified();
}
