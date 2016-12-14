#include "core-atomic.h"


/** cortex M atomic operations */
#if HOST_SIM
bool atomic_increment(unsigned &alignedDatum){
  ++alignedDatum;
  return false;
}

bool atomic_decrement(unsigned &alignedDatum){
  --alignedDatum;
  return false;
}

bool atomic_decrementNotZero(unsigned &alignedDatum){
  if(alignedDatum) {
    --alignedDatum;
  }
  return false;
}

bool atomic_incrementNotMax(unsigned &alignedDatum){
  if(alignedDatum != 0xffffffff) {
    ++alignedDatum;
  }
  return false;
}

bool atomic_setIfZero(unsigned &alignedDatum, unsigned value){
  if(alignedDatum == 0){
    alignedDatum=value;
  }
  return false;
}
#else // real code
// assembler file will supply code
#endif // if HOST_SIM


