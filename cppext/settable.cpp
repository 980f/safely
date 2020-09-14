#include "settable.h"

//temporarily don't have a project options file for this:
#ifndef MAXARGS
#define MAXARGS 15
#endif

//bool Settable::blockCheck(Settable &desired) const{
//  return differs(desired);
//}
//
//bool Settable::differs(const Settable &other)const{
//  ArgBlock<MAXARGS> args;
//  ArgBlock<MAXARGS> args2;
//
//  other.getParams(args);
//  this->getParams(args2);
//  return !args2.equals(args);
//}

void Settable::copy(const Settable &other){
  if(!&other){//#warn -Waddress etc. is ok, we guard against a compiler generated operator = with a null 'other'
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
