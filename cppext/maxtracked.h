#ifndef MAXTRACKED_H
#define MAXTRACKED_H "(C) Andrew L. Heilveil, 2017"

#include "extremer.h"

template <typename Scalar> class MaxTracked:public SimpleExtremer<Scalar,false,false>{
  Scalar latestValue;

  using Base = SimpleExtremer<Scalar,false,false>;
public:
  /** @param initValue is a starting value, @param andInspect is whether to including this in the determination of maximum. A reason to NOT include it in the tracking is if it is a default value to use when nothing was presented to the tracker and the point of use didn't check like is should.*/
  MaxTracked(Scalar initValue=0,bool andInspect=true){
    latestValue=initValue;
    if(andInspect){
      Base::inspect(initValue);
    }
  }

  /** @returns the value received so that this object can be used in a pass through situation such as somefun(object=newest) when somefun should see the new value.
   * This was buggy until late in 2024, unless template type was unsigned or value compatible with that type.
   */
  Scalar operator =(Scalar value){
    latestValue=value;
    Base::inspect(value);
    return latestValue;
  }

  /** @returns last value, @see operator = for why we do this. */
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
