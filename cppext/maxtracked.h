#ifndef MAXTRACKED_H
#define MAXTRACKED_H

#include "extremer.h"
//will template soon
class MaxTracked:public SimpleExtremer<unsigned,false,false>{
  unsigned latestValue;
public:
  MaxTracked(unsigned initValue){
    inspect(initValue);
  }

  MaxTracked()=default;

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
