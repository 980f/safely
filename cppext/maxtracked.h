#ifndef MAXTRACKED_H
#define MAXTRACKED_H

#include "extremer.h"
//will template soon
class MaxTracked:public SimpleExtremer<unsigned,false,false>{
  unsigned latestValue;
public:
  /** @param initValue is a starting value, @param andInspect is whether to including this in the determination of maximum */
  MaxTracked(unsigned initValue=0,bool andInspect=true){
    latestValue=initValue;
    if(andInspect){
      inspect(initValue);
    }
  }

  unsigned operator =(unsigned value){
    latestValue=value;
    inspect(value);
    return latestValue;
  }

  operator unsigned ()const {
    return latestValue;
  }

public: //accessors in case we make the backing fields protected.
  bool hasValue(){
    return started;
  }

  unsigned max()const {
    return extremum;
  }
};

#endif // MAXTRACKED_H
