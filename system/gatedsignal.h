#ifndef GATEDSIGNAL_H_
#define GATEDSIGNAL_H_

#include "sigcuser.h"

/**
 *  Wraps a SimpleSignal and provides for multiple entities to defer triggering of the signal.
 *  A counter tracks unbalanced gate() and ungate() calls. When the count is zero send() will emit the signal.
 *  When the count is not zero send() sets a flag that when the count becomes zero is checked and if true then the signal is emitted.
 *
 *  The number of send() calls while gated off is not recorded, multiple attempts result in a single emit() when gating is finally off.
 *
 */
class GatedSignal {
public:
  GatedSignal();
  SimpleSignal::iterator connect(const SimpleSlot &_slot);
  /** emits signal if not gated off else arranges for signal to emit when gated back on.*/
  void send();
  /** defer any sends until after matching ungate() has been called.
   *  You should use a Freezer instead of an explicit call to this method */
  void gate();
  /** negate previous call to gate(), send signal if send() was called while gated off.
   *  You should use a Freezer instead of an explicit call to this method */
  void ungate();

  /** forget send() was called while gated off, does not affect gating */
  void kill();
  /** @returns a slot that when invoked invokes calls send()*/
  SimpleSlot propagator();

  /** creating a Freezer object freezes triggering the signal, deleting object liberates the signal.
   * this guarantees ungate() gets called for each gate() called by this entity.
   * usage: GatedSignal::Freezer uselessname(someGatedSignal); instead of calling someGatedSignal::gate() and hopefully remembering to call its ungate() later. */
  class Freezer {
    GatedSignal &gs;
public:
    /** gate the signal and remember it*/
    Freezer(GatedSignal &gs);
    /** ungate the signal */
    ~Freezer();
  }; //only instantiate instance at need.

protected:
  SimpleSignal mySignal;
  int gateCounter;
  bool doEmit;
}; // class GatedSignal

/** a gated signal with an argument that is known at construction time.
 *  when the signal is fired the slots will all get the arg stored when this gatedsignal was created.
 */
template<typename Arg> class GatedSignalBound1 : public GatedSignal {
protected:
  //this argument will be bound to each one-arg slot that gets connected.
  Arg &knownArg;

public:
  GatedSignalBound1(Arg &knownArg) : knownArg(knownArg){

  }

  SimpleSignal::iterator connect(const sigc::slot< void,Arg > &_slot){
    return GatedSignal::connect(sigc::bind(_slot,ref(knownArg)));
  }

}; // class GatedSignalBound1

#endif // _GATEDSIGNAL_H_
