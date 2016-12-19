#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/** like strchr but with idiot checks on the parameters */
bool isPresent(const char *flags, char flag);



/** creation of one of these increments the related integer, destroying one decrements it.
 * Used as a base class, with argument a relatively persistant item to count the number of extant objects of that class.*/
class CountedFlagger {
  int &flag;
public:
  CountedFlagger(int &flag):flag(flag){
    ++flag;
  }

  ~CountedFlagger(){
    --flag;
  }
};

/** @returns whether assigning @param newvalue to @param target changes the latter. the compare is for nearly @param bits, not an exact number. If nearly the same then the assignment does not occur.
This is handy when converting a value to ascii and back, it tells you whether that was significantly corrupting.
*/
bool changed(double&target, double newvalue,int bits=32);

/** @returns whether assigning @param newvalue to @param target changes the latter */
template <typename Scalar1, typename Scalar2> bool changed(Scalar1 &target, const Scalar2 &newvalue){
  if(target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template <typename Scalar> class ClearOnExit {
  Scalar&zipperatus;
public:
  ClearOnExit(Scalar & toBeCleared): zipperatus(toBeCleared){
    //#nada
  }
  ClearOnExit(Scalar & toBeCleared,Scalar setnow): zipperatus(toBeCleared){
    zipperatus=setnow;
  }

  operator Scalar(void){
    return zipperatus;
  }

  ~ClearOnExit (){
    zipperatus = 0;
  }
};

/** Clears a flag when destroyed */
class AutoFlag: public ClearOnExit<bool> {
public:
  AutoFlag(bool & toBeCleared);
};

#define SETGUARD(boolvarb) AutoFlag coe_##boolvarb(boolvarb)

/** assign a value to variable on block exit, regardless of how the exit happens, including exceptions.
  * NB: records value to use at time of this object's creation */
template <typename Scalar> class AssignOnExit {
  Scalar&zipperatus;
  Scalar onexit;
public:
  AssignOnExit(Scalar & toBeCleared, Scalar onexit): zipperatus(toBeCleared), onexit(onexit){}
  ~AssignOnExit (){
    zipperatus = onexit;
  }
  operator Scalar() const {
    return zipperatus;
  }
  Scalar delta(void){
    return onexit - zipperatus;
  }
};

/** assign new value but return previous, kinda like value++ */
template <typename Scalar> Scalar postAssign(Scalar&varb, Scalar value){
  Scalar was = varb;
  varb = value;
  return was;
}

/** atomic test-and-clear */
template <typename Scalar> Scalar flagged(Scalar&varb) ISRISH;

template <typename Scalar> Scalar flagged(Scalar&varb) {
/** this implementation isn't actually atomic, we managed to single thread our whole codebase. */
  Scalar was = varb;
  varb = 0;
  return was;
}

////custom instantiation for breakpointing on change.
//inline bool flagged(bool &varb){
//  if(varb){
//    varb=0;
//    return true;
//  } else {
//    return false;
//  }
//}

/** if arg is false set it to true and return true else return false.
 wrapper for operation in case we have to make it truly atomic. */
inline bool notAlready(bool &varb){
  if(!varb){
    varb=true;
    return true;
  } else {
    return false;
  }
}

#endif // CHEAPTRICKS_H
