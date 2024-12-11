#pragma once

/** helpers to make sure something gets done regardless of how the enclosing block exits.
 * This is C++'s answer to Java's "try with resources" and similar features in other languages.
*/

/** ModifyOnExit is a base class, of no direct use. */
template<typename Scalar> class ModifyOnExit {
protected:
  Scalar &target;

  ModifyOnExit(Scalar &toBeCleared) : target(toBeCleared) {
    //#nada
  }

  ModifyOnExit(Scalar &toBeCleared, Scalar setnow) : target(toBeCleared) {
    target = setnow;
  }

public:
  operator Scalar() {
    return target;
  }

  /** overload this to do something to the saved reference on exit */
  virtual ~ModifyOnExit() = default;
};

/** assign a value to variable on block exit, regardless of how the exit happens, including exceptions.
 * NB: it records the value to use at time of this object's creation */
template<typename Scalar> class AssignOnExit : public ModifyOnExit<Scalar> {
protected:
  using ModifyOnExit<Scalar>::ModifyOnExit;
  using ModifyOnExit<Scalar>::target;
  /** value to assign to target on exit */
  Scalar onexit;

public:
  AssignOnExit(Scalar &target, Scalar onexit) : ModifyOnExit<Scalar>(target) {
    this->onexit = onexit;
  }

  ~AssignOnExit() override {
    target = onexit;
  }

  /** @returns the change that would occur if you exit now */
  Scalar delta() const noexcept {
    return onexit - target;
  }
};

/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template<typename Scalar> class ClearOnExit : public AssignOnExit<Scalar> {
public:
  ClearOnExit(Scalar &thing): AssignOnExit<Scalar>(thing, 0) {}
};

/** record present value to be restored on exit, assign a new value at construction */
template<typename Scalar> class Stacked : public AssignOnExit<Scalar> {
  Stacked(Scalar &target, Scalar newvalue): AssignOnExit<Scalar>(target, target) {
    target = newvalue;
  }
};

/** assign a value to variable from another one on block exit, regardless of how the exit happens, including exceptions.
 * NB: the value set will be the value of the @param onexit when the exit occurs. If that item is dynamically allocated then it might get freed before this object copies it, so it will minimize but not eliminate bugs if you use a 'zero/Nan on delete' method to free such a thing. */
template<typename Scalar> class CopyOnExit : public ModifyOnExit<Scalar> {
  using ModifyOnExit<Scalar>::target;
  Scalar &onexit;

public:
  CopyOnExit(Scalar &toBeCleared, Scalar &onexit) : ModifyOnExit<Scalar>(toBeCleared),
    onexit(onexit) {
    //both are references, you should not delete either of them until after the scope of this object is exited.
  }

  ~CopyOnExit() {
    target = onexit;
  }

  operator Scalar() const noexcept {
    return target;
  }

  /** @returns the change that would occur to the target should the exit occur now */
  Scalar delta() const noexcept {
    return onexit - target;
  }
}; // class AssignOnExit


/** Clears a flag when destroyed */
class AutoFlag : public ClearOnExit<bool> {
public:
  AutoFlag(bool &toBeCleared);
};

#ifndef SETGUARD
#define SETGUARD(boolvarb) AutoFlag coe_ ## boolvarb(boolvarb)
#endif

template<typename Scalar> class IncrementOnExit : public ModifyOnExit<Scalar> {
public:
  using ModifyOnExit<Scalar>::ModifyOnExit; //needed to get a default constructor
  using ModifyOnExit<Scalar>::target;

  virtual ~IncrementOnExit() {
    ++target;
  }
};

/** creation of one of these increments the related integer, destroying one decrements it.
 * Used as a base class, with argument a relatively persistant item to count the number of extant objects of that class.
 *  once upon a time there was a duplicate class CountedFlagger
 *
 *  This is all inlined because the compiler can optimize it to just the inc and dec code. Once LTO can do that we can put the class out-of-line.
 */
class CountedLock {
  unsigned &counter;

public:
  CountedLock(unsigned &counter) : counter(counter) {
    ++counter;
  }

  ~CountedLock() {
    --counter;
  }

  /** can read but not write via this class.*/
  operator unsigned() const {
    return counter;
  }
}; // class CountedLock


/** usage: DeleteOnExit<typeofinstance>moriturus(&instance);
 *  for functions with multiple exits, or that might get hit with exceptions.
 *  NB: you must name an instance else it immediately deletes after construction.
 *
 * You can DeleteOnExit <X> shortliveditem( new X()) and when that item goes out of scope it will be deleted.
 * That is almost the same as X shortliveditem(), but allocates on the heap rather than the stack.
 */
template<typename Deletable> class DeleteOnExit {
  Deletable *something;

public:
  DeleteOnExit(Deletable *something) : something(something) {
    //we have recorded that which is to be deleted.
  }

  DeleteOnExit(Deletable &something) : something(&something) {
    //we have recorded that which is to be deleted.
  }

  /** named version of cast to template type */
  Deletable &object() {
    return *something;
  }

  /** cuteness, that lets us actually use the DOR object instead of getting a warning */
  operator Deletable &() {
    return object();
  }

  operator Deletable *() {
    return something;
  }

  operator const Deletable &() const {
    return *something;
  }

  /** @returns whether there is an object lurking inside of this */
  operator bool() const {
    return something != nullptr;
  }

  /** this class exists to execute this method*/
  ~DeleteOnExit() {
    delete something;
  }
}; // class DeleteOnReturn


/** while this could be used as the base for most of the other onexit.h classes they are simple enough to keep 'hard coded'*/

#if __has_include(<functional>)
#include <functional>

class OnExit {
  using Lamda = std::function<void()>;
  Lamda lamda;

public:
  OnExit(Lamda dolater): lamda(dolater) {
    //#nada
  }

  ~OnExit() {
    lamda();
  }
};
#else
class OnExit {
  using SimpleFunction = void(*)();
  SimpleFunction deffered;
public:
  OnExit(const SimpleFunction &deffered):deffered(deffered){}
  ~OnExit() {
    deffered();
  }
};

#endif
