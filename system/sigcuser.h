#pragma once

/** sigc is better than C++ functional as it has sigc::trackable and signals which manage using deletable items in slots
 * version 3.0: arg list to slot and signal need function syntax: rettype,argtype -> rettype(argtype)
 * sigc::ref replaced with std::ref
 *
*/

#include <sigc++/sigc++.h>
#include "localonexit.h" //for deleteonreturn

//sigc trackable should be inherited from virtually in so many cases that we always shall
#define SIGCTRACKABLE virtual public sigc::trackable

//these describe functions
#define MyHandler(membptr) sigc::mem_fun(*this, &membptr)

//these record arguments to use, rather than ones that will be filled in at time of call.
#define CallThis(memberfun,...)  connect(sigc::bind(sigc::mem_fun(this,&memberfun), ## __VA_ARGS__ ))
#define Call(object,memberfun,...) connect(sigc::bind(sigc::mem_fun(object,&memberfun), ## __VA_ARGS__ ))


using SimpleSlot = sigc::slot<void()>;//sigc 3.0 needed some help here
using SimpleSignal = sigc::signal<void()>;//sigc 3.0 needed some help here

using BooleanSlot = sigc::slot<void(bool)>;
using BooleanSignal = sigc::signal<void(bool)>;

/** signal.connect(invertSignal(someslot)) someslot receives complement of what signal sends:*/
BooleanSlot invertSignal(BooleanSlot slot);

/** to stub out a sigc::slot<bool>: */
bool alwaysTrue();

template<typename T> T regurgitate(T copyable){
  return copyable;
}

/** useful for filters: accept anything*/
template<typename T> bool always(bool b,T /*ignored*/){
  return b;
}

#if NO_VARIADIC_TEMPLATES
/** an adaptor to add a fixed return value to a slot that didn't have one.*/
template<typename T> T callAndReturn(SimpleSlot &voidly,T fixedReturn){
  voidly();
  return fixedReturn;
}

/** @returns a slot that when invoked returns @param fixedReturn*/
template<typename T> sigc::slot<T> addReturn(SimpleSlot &voidly,T fixedReturn){
  return sigc::bind(&callAndReturn<T>,voidly,fixedReturn);//#do NOT use ref here, let original slot evaporate.
}

/** an adaptor to add a fixed return value to a slot that didn't have one.*/
template<typename T,typename A> T call1AndReturn(sigc::slot<void(A)> &voidly,T fixedReturn){
  voidly();
  return fixedReturn;
}

template<typename T,typename A> sigc::slot<T,A> addReturn1(sigc::slot<void(A)> &voidly,T fixedReturn){
  return sigc::bind(&call1AndReturn<T,A>,voidly,fixedReturn);//#do NOT use ref here, let original slot evaporate.
}

/** creates a slot that when executed/invoked removes that slot from any signal.*/
//class RunOnceSlot : SIGCTRACKABLE {
//  SimpleSlot action;
//  /** construction of a useful one requires a little post construction work, so use @see makeInstance */
//  RunOnceSlot(SimpleSlot action);
//  void run();
//public:
//  /** make a new self deleting slot runner.  */
//  static SimpleSlot makeInstance(SimpleSlot action);
//};

#else // if NO_VARIADIC_TEMPLATES
//now that we have variadic templates tamed:
/** an adaptor to add a fixed return value to a slot that didn't have one.*/
template<typename T,typename ... Args> T call1AndReturn(sigc::slot<void(Args ...)> &voidly,T fixedReturn){
  voidly();
  return fixedReturn;
}

template<typename T,typename ... Args> sigc::slot<T(Args ...)> addReturn(sigc::slot<void(Args ...)> &voidly,T fixedReturn){
  return sigc::bind(&call1AndReturn<T,Args ...>,voidly,fixedReturn);//#do NOT use ref here, let original slot evaporate.
}

#endif // if NO_VARIADIC_TEMPLATES


/** adaptor to call a function and assign it to a stored native-like target */
template<typename T> void assignTo(T &target,sigc::slot<T()> getter){
  target = getter();
}

template<typename T> void assignValueTo(T value, T &target){
  target = value;
}

BooleanSlot assigner(bool &target);

/** for use bound into a slot, when invoked it calls the action if the @param source returns the @param edge */
void onEdge(sigc::slot<bool()> source,bool edge,SimpleSlot action);

/** a slot that runs once, via deleting itself when run.
 * It is inadvisable to keep a reference to one of these, expected use to use @see makeInstance in the argumentlist of a signal.connect() call.
 */
template< typename ... Args> class RunOnceSlot : SIGCTRACKABLE {
  using Action = sigc::slot< void(Args ... )>;
  Action action;
  RunOnceSlot(Action slot) :
    action(slot){
  }

  void run(Args ... args){
    DeleteOnExit< RunOnceSlot> dor(this);
    action(args ...);
  }

public:
  /** make a new self deleting slot runner */
  static Action makeInstance(Action slot){
    RunOnceSlot *dou(new RunOnceSlot(slot));
    return sigc::mem_fun(dou, &RunOnceSlot::run);
  }

}; // class RunOnceSlot


/** usage: Finally d(slot);
 *  slot executes when 'd' goes out of scope.
 */
class Finally {
  SimpleSlot action;
public:
  Finally(const SimpleSlot &action);
  ~Finally();
};

/** usage:  ConnectionLocker locked(connection); if(locked){ ... } */
class ConnectionLocker {
  bool wasBlocked;
  sigc::connection &conn;
public:
  ConnectionLocker(sigc::connection &conn);
  ~ConnectionLocker();
  operator bool();
};


/** a signal accumulator that invokes all slots, anding the result */
struct AndAll {
  typedef bool result_type;//#needed for sigc magic
  template<class I> //#sigc will insert an iterator type
  result_type operator()(I it, I end) const {
    bool sum = true;
    while(it != end) {
      if(!*it++) {
        sum = false;//#leave this expanded so that we can breakpoint here.
      }
    }
    return sum;
  }

};

/** This is a short-circuit accumulator */
struct AndUntilFalse {
  typedef bool result_type;
  template<typename T_iterator> result_type operator()(T_iterator first, T_iterator last) const {
    while(first != last) {
      if (!*first++) {
        return false;
      }
    }
    return true;
  }

};
