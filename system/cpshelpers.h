#ifndef _CPSHELPERS_H_
#define _CPSHELPERS_H_

#include <sigcuser.h>


/** execute function after given (and manipulatable) number of 'hits' */
class Barrier {
  sigc::slot< void > continuation;
  int count;

  Barrier(const sigc::slot< void > continuation, int count);
public:
  static Barrier &getInstance(const sigc::slot< void > continuation, int count);
  /** decrement count and on zero execute stored function, then delete's self so you may not refer to the barrier after hit()ing it.*/
  void hit(int hits = 1);
  void unhit(int hits = 1);
  /** creates a defered execution of hitting this barrier, @param unhit executes a matching unhit() */
  sigc::slot< void > make_hitter(int hits = 1,bool unhitit = true);
  void connect_single_shot_hitter(sigc::signal< void > sig, int hits = 1);
}; // class Barrier

#endif // _CPSHELPERS_H_
