#include "safely.h"
#include "roundrobiner.h"
#include "cheaptricks.h"
#include "minimath.h" //fill's clear's

RoundRobiner::RoundRobiner(bool *Scoreboard, unsigned quantity) : Scoreboard(Scoreboard), quantity(quantity){
  markAll(0);
  last = BadLength;
}

bool *RoundRobiner::deallocate(){
  last = BadLength;
  quantity = 0;
  return Scoreboard;
}

/** carefully crafter to ensure that even if the scoreboard is set fully between calls to this function this function will once per cycle return 'nothing'*/
unsigned RoundRobiner::next(void){
  for(unsigned n = last; ++n < quantity; ) {
    if(flagged(Scoreboard[n])) {
      return last = n; //#assign and return
    }
  }
  return last = BadLength; //magic value for 'none'
} /* next */

bool &RoundRobiner::bit(unsigned id) const {
  static bool trashme;
  if(id >= quantity) {
    return trashme;
  }
  return Scoreboard[id];
}

bool RoundRobiner::post(unsigned id){
  if(id==ALL) {//hack for occasional "reportAll" functionality.
    markAll();
    return true;
  }
  if(id >= quantity) {
    return false;
  }
  Scoreboard[id] = true;
  return true;
} // RoundRobiner::post

void RoundRobiner::markAll(bool thesame){
  fillObject(Scoreboard,quantity * sizeof(bool),thesame);
  //#leave last alone! resetting it here will short the cycle if markAll is called faster than the queue can empty.
}
