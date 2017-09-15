#ifndef ROUNDROBINER_H
#define ROUNDROBINER_H

/** a bit map for use in round-robin prioritization of a set of actions. Limited to the number of bit in an unsigned.
 * one could use 'largest native integral type' as that appears to now be a standard token, somewhere in some std library.
  */
#include "bitbanger.h" //to pass back reference to bit.
class RoundRobiner {
  unsigned bits;
  unsigned quantity;
  unsigned last;
public:
  RoundRobiner(unsigned quantity);

  /** for cached access to a postable bit, write a 1 to "post"*/
  BitReference bit(unsigned id);
  bool bit(unsigned id) const;

  static const unsigned ALL=~0U;
  /** set bit and @return whether was already posted */
  bool post(unsigned id);
  /** @return index of next set bit, clear bit on return
    * usage: value=rr.next(); if(value>=0){ do something with value } else //there was nothing to do.
    */
  unsigned next(void);
  //set all flags, point 'last' to make 0th item be 'next'
  void markAll(bool thesame=1);
};

#endif // ROUNDROBINER_H
