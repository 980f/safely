#ifndef SIGCUSER_H
#define SIGCUSER_H

#include <sigc++/sigc++.h>
#include "cheaptricks.h" //for deleteonreturn

//sigc trackable should be inherited from virtually in so many cases that we always shall
#define SIGCTRACKABLE virtual public sigc::trackable

//these describe functions
#define MyHandler(membptr) sigc::mem_fun(this, &membptr)

//these record arguments to use, rather than ones that will be filled in at time of call.
#define CallThis(memberfun,...)  connect(sigc::bind(sigc::mem_fun(this,&memberfun), ## __VA_ARGS__ ))
#define Call(object,memberfun,...) connect(sigc::bind(sigc::mem_fun(object,&memberfun), ## __VA_ARGS__ ))


typedef sigc::slot<void> SimpleSlot;
typedef sigc::signal<void> SimpleSignal;

typedef sigc::slot<void,bool> BooleanSlot;
typedef sigc::signal<void,bool> BooleanSignal;

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
template<typename T,typename A> T call1AndReturn(sigc::slot<void,A> &voidly,T fixedReturn){
  voidly();
  return fixedReturn;
}

template<typename T,typename A> sigc::slot<T,A> addReturn1(sigc::slot<void,A> &voidly,T fixedReturn){
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
template<typename T,typename ... Args> T call1AndReturn(sigc::slot<void,Args ...> &voidly,T fixedReturn){
  voidly();
  return fixedReturn;
}

template<typename T,typename ... Args> sigc::slot<T,Args ...> addReturn(sigc::slot<void,Args ...> &voidly,T fixedReturn){
  return sigc::bind(&call1AndReturn<T,Args ...>,voidly,fixedReturn);//#do NOT use ref here, let original slot evaporate.
}

#endif // if NO_VARIADIC_TEMPLATES


/** adaptor to call a function and assign it to a stored native-like target */
template<typename T> void assignTo(T &target,sigc::slot<T> getter){
  target = getter();
}

template<typename T> void assignValueTo(T value, T &target){
  target = value;
}

BooleanSlot assigner(bool &target);

/** for use bound into a slot, when invoked it calls the action if the @param source returns the @param edge */
void onEdge(sigc::slot<bool> source,bool edge,SimpleSlot action);


template< typename ... Args> class RunOnceSlot : SIGCTRACKABLE {
  typedef sigc::slot< void, Args ... > Slot;
  Slot slot;
  RunOnceSlot(Slot slot) :
    slot(slot){
  }

  void run(Args ... args){
    DeleteOnReturn< RunOnceSlot< Args ... >> dor(this);
    slot(args ...);
  }

public:
  /** make a new self deleting slot runner */
  static Slot makeInstance(Slot slot){
    RunOnceSlot &dou(*new RunOnceSlot(slot));
    return mem_fun(dou, &RunOnceSlot::run);
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

//these required Glib stuff --
//void doSoon(SimpleSlot slot,int howSoon = 0,int howurgently = 1);
///** @return a slot that when invoked will schedule execution of the @param given slot.
// *  deleting objects referenced by the toDefer slot should be interesting ;) */
//SimpleSlot eventually(SimpleSlot toDefer);

/** a signal accumulator that invokes all slots, anding the result */
struct AndAll {
  typedef bool result_type;//#needed for sigc magic
  template<class I> //#sigc will insert an iterator type
  result_type operator()(I it, I end) const {
    bool sum = true;
    while(it != end) {
      if(!*it++) {
        sum = false;
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

#endif // ifndef SIGCUSER_H
