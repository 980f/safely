#pragma once

#include "sigcuser.h"
/** Provides for invoking an operation once even though multiple rules might invoke it. Create a Freezer at the beginning of a block of code that might trigger the signal multiple times. It will only run at most once when the Freezer is deleted.
 *
 *  Wraps a SimpleSignal and provides for multiple entities to defer triggering of the signal.
 *  A counter tracks unbalanced gate() and ungate() calls. When the count is zero send() will emit the signal.
 *  When the count is not zero send() sets a flag that when the count becomes zero is checked and if true then the signal is emitted.
 *
 *  The number of send() calls while gated off is not recorded, multiple attempts result in a single emit() when gating is finally off.
 *
 * The slot cannot have any arguments as the send only sends when not gated, which argument would it use?
 */

class GatedSignal {
public:
  GatedSignal();
  /** registers */
  SimpleSignal::iterator connect(const SimpleSlot &slot);
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

    /** a copy, that doesn't trigger the signal */
    Freezer(Freezer &&generated);
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
 *
 * If each slot needs its own argument simple bind those and use a GatedSignal.
 */
template<typename Arg> class GatedSignalBound : public GatedSignal {
protected:
  /** this argument will be bound to each one-arg slot that gets connected. */
  Arg &knownArg;

public:
  GatedSignalBound(Arg &knownArg) : knownArg(knownArg){

  }

  SimpleSignal::iterator connect(const sigc::slot< void,Arg > &slot){
    return GatedSignal::connect(sigc::bind(slot,ref(knownArg)));//use a seperate sigc::ref for each slot
  }

};
