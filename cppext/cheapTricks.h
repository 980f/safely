#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

bool isPresent(const char *flags, const char flag);

bool changed(double&target, double newvalue,int bits=32);

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
/** this implementation isn't actually atomic, we managed to single thread our whole codebase */
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
