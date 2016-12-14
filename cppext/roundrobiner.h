#ifndef ROUNDROBINER_H
#define ROUNDROBINER_H

/** a bit map for use in round-robin prioritization of a set of actions
  * maydo: reform using Indexer, or create templated size wrapper.
  */

class RoundRobiner {
  bool *Scoreboard;
  int quantity;
  int last;
public:
  RoundRobiner(bool * Scoreboard, int quantity);
  /** one parent dynamically allocated the scoreboard. */
  bool *deallocate();
  /** for cached access to a postable bit, write a 1 to "post"*/
  bool &bit(int id)const;
  /** set bit and @return whether was already posted */
  bool post(int id);
  /** @return index of next set bit, clear bit on return
    * usage: value=rr.next(); if(value>=0){ do something with value } else //there was nothing to do.
    */
  int next(void);
  //set all flags, point 'last' to make 0th item be 'next'
  void markAll(bool thesame=1);
};

#endif // ROUNDROBINER_H
