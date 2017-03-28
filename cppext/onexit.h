#ifndef ONEXIT_H
#define ONEXIT_H

/** arrange to do things when exiting block scope */

/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template <typename Scalar> class ClearOnExit {
  Scalar&zipperatus;
public:
  ClearOnExit(Scalar & toBeCleared): zipperatus(toBeCleared){}
  operator Scalar(void){
    return zipperatus;
  }

  ~ClearOnExit (){
    zipperatus = 0;
  }
};


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
  /** access wrapped entity via this object, handy when wrapping a dynamically selected item */
  operator Scalar() const {
    return zipperatus;
  }
  /** @returns eventual value - present value , the change that will be imposed at exit */
  Scalar delta(void){
    return onexit - zipperatus;
  }
};

/** form of AssignOnExit for use in a return statement: */
template <typename Scalar> Scalar postAssign(Scalar&varb, Scalar value){
  Scalar was = varb;
  varb = value;
  return was;
}

#endif // ONEXIT_H
