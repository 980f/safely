#ifndef MAXTRACKED_H
#define MAXTRACKED_H "(C) Andrew L. Heilveil, 2017"

#include "extremer.h"

template <typename Scalar> class MaxTracked:public SimpleExtremer<Scalar,false,false>{
  Scalar latestValue;

  using Base = SimpleExtremer<Scalar,false,false>;
public:
  /** @param initValue is a starting value, @param andInspect is whether to including this in the determination of maximum */
  MaxTracked(Scalar initValue=Scalar(),bool andInspect=true){
    latestValue=initValue;
    if(andInspect){
      Base::inspect(initValue);
    }
  }

  unsigned operator =(Scalar value){
    latestValue=value;
    Base::inspect(value);
    return latestValue;
  }

  operator Scalar()const {
    return latestValue;
  }

public: //accessors in case we make the backing fields protected.
  bool hasValue(){
    return Base::started;
  }

  Scalar max()const {
    return Base::extremum;
  }
};

#endif // MAXTRACKED_H
