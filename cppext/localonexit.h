#pragma once

/** helpers to make sure something gets done regardless of how the enclosing block exits.
 * This is C++'s answer to Java's "try with resources" and similar features in other languages.
*/

template<typename Scalar> class ModifyOnExit {
protected:
  Scalar &zipperatus;
public:
  ModifyOnExit(Scalar & toBeCleared) : zipperatus(toBeCleared){
    //#nada
  }

  ModifyOnExit(Scalar & toBeCleared,Scalar setnow) : zipperatus(toBeCleared){
    zipperatus = setnow;
  }

  operator Scalar(void){
    return zipperatus;
  }

  virtual ~ModifyOnExit()=default;

}; // class ClearOnExit


/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template<typename Scalar> class ClearOnExit :public ModifyOnExit<Scalar> {
public:
  using ModifyOnExit<Scalar>::ModifyOnExit;
  using ModifyOnExit<Scalar>::zipperatus;

  ~ClearOnExit(){
    zipperatus = 0;
  }

}; // class ClearOnExit


/** Clears a flag when destroyed */
class AutoFlag : public ClearOnExit<bool> {
public:
  AutoFlag(bool &toBeCleared);
};

#ifndef SETGUARD
#define SETGUARD(boolvarb) AutoFlag coe_ ## boolvarb(boolvarb)
#endif

template<typename Scalar> class IncrementOnExit:public ModifyOnExit<Scalar> {
public:
  using ModifyOnExit<Scalar>::ModifyOnExit;
  using ModifyOnExit<Scalar>::zipperatus;

  virtual ~IncrementOnExit(){
    ++zipperatus;
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
  CountedLock(unsigned &counter) : counter(counter){
    ++counter;
  }

  ~CountedLock(){
    --counter;
  }

  /** can reference but not alter via this class.*/
  operator unsigned() const {
    return counter;
  }
}; // class CountedLock


/** assign a value to variable on block exit, regardless of how the exit happens, including exceptions.
 * NB: it records the value to use at time of this object's creation */
template<typename Scalar> class AssignOnExit:public ModifyOnExit<Scalar> {
  using ModifyOnExit<Scalar>::ModifyOnExit;
  using ModifyOnExit<Scalar>::zipperatus;

  Scalar onexit;
public:
  AssignOnExit(Scalar & toBeCleared, Scalar onexit) : ModifyOnExit<Scalar>(toBeCleared){
    this->onexit=onexit;
  }

  ~AssignOnExit(){
    zipperatus = onexit;
  }

  /** @returns the change that would occur if you exit now */
  Scalar delta(void) const noexcept{
    return onexit - zipperatus;
  }

}; // class AssignOnExit

/** assign a value to variable from another one on block exit, regardless of how the exit happens, including exceptions.
 * NB: the value set will the value of the @param onexit when the exit occurs. If that item is dynamically allocated then it might get freed before this object copies it. */
template<typename Scalar> class CopyOnExit {
  Scalar&zipperatus;
  Scalar& onexit;
public:
  CopyOnExit(Scalar & toBeCleared, Scalar &onexit) : zipperatus(toBeCleared), onexit(onexit){
    //both are references, you should not delete either of them until after the scope of this object is exited.
  }

  ~CopyOnExit(){
    zipperatus = onexit;
  }

  operator Scalar() const noexcept{
    return zipperatus;
  }
  /** @returns the change that would occur to the target should the exit occur now */
  Scalar delta(void) const noexcept{
    return onexit - zipperatus;
  }

}; // class AssignOnExit

/** usage: DeleteOnExit<typeofinstance>moriturus(&instance);
 *  for functions with multiple exits, or that might get hit with exceptions.
 *  NB: you must name an instance else it immediately deletes after construction.
 *
 * You can DeleteOnExit <X> shortliveditem( new X()) and when that item goes out of scope it will be deleted.
 * That is almost the same as X shortliveditem(), but allocates on the heap rather than the stack.
 */
//legacy name was at odds with other similar functionality class' names
#define DeleteOnReturn DeleteOnExit
template<typename Deletable> class DeleteOnExit {
  Deletable*something;
public:
  DeleteOnExit(Deletable*something) : something(something){
    //we have recorded that which is to be deleted.
  }

  DeleteOnExit(Deletable&something) : something(&something){
//we have recorded that which is to be deleted.
  }

  /** named version of cast to template type */
  Deletable &object(){
    return *something;
  }

  /** cuteness, that lets us actually use the DOR object instead of getting a warning */
  operator Deletable &(){
    return object();
  }

  operator Deletable *(){
    return something;
  }

  operator const Deletable &() const {
    return *something;
  }

  /** @returns whether there is an object lurking inside of this*/
  operator bool() const {
    return something!=nullptr;
  }

  /** this class exists to execute this method*/
  ~DeleteOnExit(){
    delete something;
  }

}; // class DeleteOnReturn


/** while this could be used as the base for most of the other localonexit.h classes they are simple enough to keep 'hard coded'*/
class LocalOnExit {
  typedef void (*SimpleFunction)();
  SimpleFunction deffered;
public:
  LocalOnExit(const SimpleFunction &deffered);
  ~LocalOnExit();
};
