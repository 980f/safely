#ifndef RUNONCENTHTIME_H
#define RUNONCENTHTIME_H

#include "sigcuser.h"
/** execute function after given (and manipulatable) number of 'hits'.
 *  Inspired by DAB*/
template<typename ... Args> class RunOnceNthTime : SIGCTRACKABLE {
  typedef sigc::slot< void, Args ... > Action;
  Action action;
  int count;

public: //while the main purpose is a slot that eventually fires and dies this class can be used directly as well as in a signal
  /** @see makeInstance */
  RunOnceNthTime(Action action, int after) : action(action),count(after){
    //if after is negative this thing will fire on first hit.
  }

  void run(Args ... args){
    if(--count <= 0) {
      DeleteOnReturn<RunOnceNthTime> dou(this);//NEW@ makeInstance()
      action(args ...);
    }
  }

public:
  /** makes a slot that after @param after/@param stepsize touches fires then deletes itself */
  static Action makeInstance(const Action &action, int after){
    auto thing = new RunOnceNthTime(action, after); //DEL@ hit()
    return sigc::mem_fun(thing, &RunOnceNthTime::run);
  }

}; // class RunOnceNthTime


template<typename ... Args> class RunEvery : SIGCTRACKABLE {
  typedef sigc::slot< void, Args ... > Action;
  Action action;
  int cycle;
  int count;

public: //while the main purpose is a slot that eventually fires and dies this class can be used directly as well as in a signal
  /** @see makeInstance */
  RunEvery(Action action, int cycle) : action(action),cycle(cycle){
    //if after is negative this thing will fire on first hit.
  }

  void run(Args ... args){
    ++count;
    if(count % cycle == 0) {
      action(args ...);
    }
  }

public:
  /** makes a slot that fires every @param cycle times the signal is emitted */
  static Action makeInstance(const Action &action, int cycle){
    auto thing = new RunEvery(action, cycle);//seems like this leaks, deleting the signal may not delete this object.
    return sigc::mem_fun(thing, &RunEvery::run);
  }

}; // class RunEvery


#endif // RUNONCENTHTIME_H
