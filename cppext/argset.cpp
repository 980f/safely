#include "argset.h"
ArgSet::ArgSet(double *d, int sizeofd) : Indexer <double> (d, sizeofd){}
/**@return whether @param changed*/
bool ArgSet::applyto(double&d){
  return changed(d, next(d));
}

/**@return whether @param changed*/
bool ArgSet::applyto(int&eye){
  double dammit = eye;

  if(applyto(dammit)) {
    eye = dammit;
    return true;
  }
  return false;
}
/**@return whether @param changed*/
bool ArgSet::applyto(bool&b){
  double dammit = b ? 1.0 : 0.0;

  if(applyto(dammit)) {
    b = dammit != 0;
    return true;
  }
  return false;
}

void ArgSet::cat(const double *prefilled, int qty){//todo:3 optimize
  while(qty-->0){
    next()=*prefilled++;
  }
}

bool ArgSet::equals(ArgSet args) const{
  ArgSet cf(*this);
  while(cf.hasNext()&&args.hasNext()){
    if(!nearly(cf.next(),args.next(),18)){
      return false;
    }
  }
  if(cf.hasNext()||args.hasNext()){
    return false;//a mismatch in numargs.
  }
  return true;
}

ConstArgSet::ConstArgSet(const double *d, int sizeofd) : Indexer <const double> (d, sizeofd){}

ConstArgSet::ConstArgSet(const ArgSet &other):Indexer<const double>(other.internalBuffer(),other.allocated()*sizeof(double)){}

ConstArgSet::ConstArgSet(const ConstArgSet &other):Indexer<const double>(other){}
