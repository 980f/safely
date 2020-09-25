#include "settable.h"

//temporarily don't have a project options file for this:
#ifndef MAXARGS
#define MAXARGS 15
#endif

void Settable::copy(const Settable &other){
  //if(!&other){//#warn -Waddress etc. is ok, we guard against a compiler generated operator = with a null 'other'
  //  return;
  //}
  ArgBlock<MAXARGS> args;
  other.getParams(args);
  args.freeze();
  ConstArgSet punned(args);
  this->setParams(punned);
}

bool Settable::changed(const Settable &other){
  copy(other);
  return wasModified();
}

Settable::~Settable(){
  //#nada
}
