#include "argset.h"
#include "cheaptricks.h" //changed()

ArgSet::ArgSet(double* d, unsigned sizeofd) : Indexer<double>(d, sizeofd){
  //#nada
}

/**@return whether @param changed*/
bool ArgSet::applyto(double &d){
  return changed(d, next(d));
}

/**@return whether @param changed*/
bool ArgSet::applyto(int &eye){
  double dammit = eye;

  if(applyto(dammit)) {
    eye = dammit;
    return true;
  }
  return false;
}

/**@return whether @param changed*/
bool ArgSet::applyto(bool &b){
  double dammit = b ? 1.0 : 0.0;

  if(applyto(dammit)) {
    b = dammit != 0;
    return true;
  }
  return false;
}

void ArgSet::cat(const double* prefilled, int qty){  //todo:3 optimize
  while(qty-->0) {
    next() = *prefilled++;
  }
}

bool ArgSet::equals(const ArgSet &args, int bits) const {
  //test size up front to avert construction of 'cf' and 'others'
  if(args.used()!=this->used()) {
    return false;
  }
  ArgSet cf(*this);
  ArgSet others(args);
  while(cf.hasNext()&&others.hasNext()) {
    if(!nearly(cf.next(),others.next(),bits)) {
      return false;
    }
  }
  return true;
}

ArgSet::~ArgSet(){
 //#nada, we wrap data, we don't own it.
}

ConstArgSet::ConstArgSet(const double* d, int sizeofd) : Indexer<const double>(d, sizeofd){
}

ConstArgSet::ConstArgSet(const ArgSet &other) : Indexer<const double>(other.internalBuffer(),other.allocated() * sizeof(double)){
}

ConstArgSet::ConstArgSet(const ConstArgSet &other) : Indexer<const double>(other){
}
