#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/** like strchr but with idiot checks on the parameters */
bool isPresent(const char *flags, char flag);


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

/** if non-zero then decrement else leave alone */
class CountDown {
  unsigned counter;
public:
  CountDown(unsigned count=0):counter(count){}

  unsigned operator =(unsigned quantity){
    return counter=quantity;
  }

  /** getter */
  operator unsigned() const {
    return counter;
  }

  /** test and decrement. the main reason this class exists is to ensure we get a decrement when we test.
   * this marks something that should probably be atomic. */
  operator bool (){
    if(counter){
      --counter;
      return true;
    } else {
      return false;
    }
  }

  bool hasNext() const {
    return counter>0;
  }

  /** just how weird can c++ be? */
  template <typename Many> Many& next(Many array[]){
    return array[counter];
  }
};

/** @returns whether assigning @param newvalue to @param target changes the latter. the compare is for nearly @param bits, not an exact number. If nearly the same then
 * the assignment does not occur.
 *  This is handy when converting a value to ascii and back, it tells you whether that was significantly corrupting.
 */
bool changed(double&target, double newvalue,int bits = 32);

/** @returns whether assigning @param newvalue to @param target changes the latter */
template<typename Scalar1, typename Scalar2> bool changed(Scalar1 &target, const Scalar2 &newvalue){
  if(target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template<typename Scalar> class ClearOnExit {
  Scalar&zipperatus;
public:
  ClearOnExit(Scalar & toBeCleared) : zipperatus(toBeCleared){
    //#nada
  }

  ClearOnExit(Scalar & toBeCleared,Scalar setnow) : zipperatus(toBeCleared){
    zipperatus = setnow;
  }

  operator Scalar(void){
    return zipperatus;
  }

  ~ClearOnExit(){
    zipperatus = 0;
  }

}; // class ClearOnExit

/** Clears a flag when destroyed */
class AutoFlag : public ClearOnExit<bool> {
public:
  AutoFlag(bool & toBeCleared);
};

#define SETGUARD(boolvarb) AutoFlag coe_ ## boolvarb(boolvarb)

/** assign a value to variable on block exit, regardless of how the exit happens, including exceptions.
 * NB: it records the value to use at time of this object's creation */
template<typename Scalar> class AssignOnExit {
  Scalar&zipperatus;
  Scalar onexit;
public:
  AssignOnExit(Scalar & toBeCleared, Scalar onexit) : zipperatus(toBeCleared), onexit(onexit){
  }

  ~AssignOnExit(){
    zipperatus = onexit;
  }

  operator Scalar() const noexcept{
    return zipperatus;
  }

  /** @returns the change that would occur if you exit now */
  Scalar delta(void) const noexcept{
    return onexit - zipperatus;
  }

}; // class AssignOnExit

/** assign a value to variable from another one on block exit, regardless of how the exit happens, including exceptions.
 * NB: it records the value to use at time of this object's creation */
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


/** marker for potential atomic value shift
 * assign new value but return previous, kinda like value++
 * X previous= postAssign<x>(thingy, newvalue);
 * previous is value of thingy before the assignment, thingy has newvalue.
*/
template<typename Scalar> Scalar postAssign(Scalar&varb, Scalar value){
  Scalar was = varb;
  varb = value;
  return was;
}

/** atomic test-and-clear */
template<typename Scalar> Scalar flagged(Scalar&varb) ISRISH; //mark as needing critical optimization

template<typename Scalar> Scalar flagged(Scalar&varb){
/** this implementation isn't actually atomic, we managed to single thread our whole codebase. */
  Scalar was = varb;
  varb = 0;
  return was;
}

/** if arg is false set it to true and return true else return false.
 *  wrapper for operation in case we have to make it truly atomic. */
inline bool notAlready(bool &varb){
  if(!varb) {
    varb = true;
    return true;
  } else {
    return false;
  }
}

/** usage: DeleteOnReturn<typeofinstance>moriturus(&instance);
 *  for functions with multiple exits, or that might get hit with exceptions.
 *  NB: you must name an instance else it immediately deletes after construction.
 *
 * You can DeleteOnReturn <X> shortliveditem( new X()) and when that item goes out of scope it will be deleted.
 * That is the same as X shortliveditem(), but allocates on the heap rather than the stack.
 */
template<typename Deletable> class DeleteOnReturn {
  Deletable*something;
public:
  DeleteOnReturn(Deletable*something) : something(something){
    //we have recorded that which is to be deleted.
  }

  DeleteOnReturn(Deletable&something) : something(&something){
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

  /** @returns whether ther is an object lurking insdie this*/
  operator bool() const {
    return something!=nullptr;
  }

  /** this class exists to execute this method*/
  ~DeleteOnReturn(){
    delete something;
  }

}; // class DeleteOnReturn


#endif // CHEAPTRICKS_H
