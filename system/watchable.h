#ifndef WATCHABLE_H
#define WATCHABLE_H

#include "sigcuser.h"
#include "cheapTricks.h"
/**
  Stange names herein are done to match StoredBoolean of which this is a spiritual base class.
we must be an instance of the scalar instead of watching one in case the entity of which the scalar is a member gets delete'd.
*/
template <typename Scalar> class Watchable: SIGCTRACKABLE {
protected:
  Scalar varb;

private:
  sigc::signal<void,Scalar> watchers;

protected:
  void post(){
    watchers(varb);
  }

public:
  Scalar set(Scalar newvalue){
    if(changed(newvalue)){
      post();
    }
    return varb;
  }

  /** if different then assign and @return true after watchers are invoked */
  bool changed(const Scalar &newvalue){
    if(::changed(varb,newvalue)){
      post();
      return true;
    }
    return false;
  }

  operator Scalar()const{
    return native();
  }

  Scalar native()const{
    return varb;
  }

private:
  /** a filter to limit when to report , @see onChangeTo */
  void gater(SimpleSlot action,Scalar gate){
    if(varb==gate){
      action();
    }
  }

public:

  sigc::connection onAnyChange(sigc::slot<void,Scalar> action,bool kickme=false){
    if(kickme){
      action(varb);
    }
    return watchers.connect(action);
  }

  sigc::connection onAnyChangeJust(SimpleSlot action,bool kickme=false){
    return onAnyChange(sigc::hide(action),kickme);
  }

  sigc::connection onChangeTo(Scalar value,sigc::slot<void,Scalar> action,bool kickme=false){
    if(kickme){
      gater(action,value);
    }
    return onAnyChange(sigc::bind(MyHandler(Watchable::gater),action,value));
  }

  Watchable(Scalar initialValue=Scalar(0)):varb(initialValue) {
    //#nada
  }

  /** support explicit delete's. not virtual as we want to keep this class as light-weight as we can. */
  ~Watchable(){

  }

  // and now for some conveniences:
  Scalar operator =(Scalar newvalue){
    return set(newvalue);
  }

  /** if adding int to float you may have to explicitly cast the int */
  Scalar operator +=(Scalar newvalue){
    return set(varb+newvalue);
  }

  /** pre increment */
  Scalar operator ++(){
    return set(varb+1);
  }

  //post increment doesn't play well with the concept of watching

//someday add compares and such: #define watchcompare(op) bool operator op (Scalar othervalue) const { return varb op othervalue;}

};

#endif // WATCHABLE_H
