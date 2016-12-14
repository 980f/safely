#include "roundrobiner.h"
#include "cheapTricks.h"
#include "minimath.h" //fill's clear's

RoundRobiner::RoundRobiner(bool *Scoreboard, int quantity): Scoreboard(Scoreboard), quantity(quantity){
  markAll(0);
  last=~0;
}

bool *RoundRobiner::deallocate(){
  last=~0;
  quantity=0;
  return Scoreboard;
}

/** carefully crafter to ensure that even if the scoreboard is set fully between calls to this function this function will once per cycle return 'nothing'*/
int RoundRobiner::next(void){
  for(int n = last; ++n < quantity; ) {
    if(flagged(Scoreboard[n])) {
      return last = n; //#assign and return
    }
  }
  return last=~0; //magic value for 'none'
} /* next */

bool &RoundRobiner::bit(int id)const{
  static bool trashme;
  if(id < 0 || id >= quantity) {
    return trashme;
  }
  return Scoreboard[id];
}

bool RoundRobiner::post(int id){
  if(id==~0){//hack for occasional "reportAll" functionality.
    markAll();
    return true;
  }
  if(id < 0 || id >= quantity) {
    return false;
  }
  Scoreboard[id]=true;
  return true;
}

void RoundRobiner::markAll(bool thesame){
  fillObject(Scoreboard,quantity*sizeof(bool),thesame);
  //#leave last alone! resetting it here will short the cycle if markAll is called faster than the queue can empty.
}

