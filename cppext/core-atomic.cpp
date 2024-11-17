#include "core-atomic.h"

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
  if(alignedDatum != ~0U) {
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

