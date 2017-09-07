#include "safely.h"
#include "roundrobiner.h"
#include "cheaptricks.h"
#include "minimath.h" //fill's clear's

RoundRobiner::RoundRobiner(unsigned quantity) :  quantity(quantity){
  markAll(0);
  last = BadLength;
}

BitReference RoundRobiner::bit(unsigned id){
  return BitReference(reinterpret_cast<u64>(&bits),id);
}

bool RoundRobiner::bit(unsigned id) const {
  return ::bit( bits,id);
}


/** carefully crafter to ensure that even if the scoreboard is set fully between calls to this function this function will once per cycle return 'nothing'*/
unsigned RoundRobiner::next(void){
  for(unsigned n = last; ++n < quantity; ) {
    if(::bit(bits,n)) {
      ::clearBit(bits,n);
      return last = n; //#assign and return
    }
  }
  return last = BadLength; //magic value for 'none'
} /* next */

//bool &RoundRobiner::bit(unsigned id) const {
//  static bool trashme;
//  if(id >= quantity) {
//    return bits[31];
//  }
//  return Scoreboard[id];
//}

bool RoundRobiner::post(unsigned id){
  if(id==ALL) {//hack for occasional "reportAll" functionality.
    markAll();
    return true;
  }
  if(id >= quantity) {
    return false;
  }
  setBit(bits,id);
  return true;
} // RoundRobiner::post

void RoundRobiner::markAll(bool thesame){
  bits=thesame?~0U:0;
  //#leave last alone! resetting it here will short the cycle if markAll is called faster than the queue can empty.
}
