#pragma once

/** arrange to do things when exiting block scope */

/** clear a variable on block exit, regardless of how the exit happens, including exceptions.
 * The variable being cleared should not be declared in the same block as this agent.
 *
 */
template <typename Scalar> class ClearOnExit {
  Scalar&zipperatus;
public:
  ClearOnExit(Scalar & toBeCleared): zipperatus(toBeCleared){}

  operator Scalar(){
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
  Scalar delta(){
    return onexit - zipperatus;
  }
};

#if __has_include<functional>
#include <functional>
struct OnExit {
  typedef std::function<void(void)> Lamda;
  Lamda lamda;
  OnExit(Lamda dolater):lamda(dolater){
    //#nada
  }

  ~OnExit(){
    lamda();
  }
};

#endif // ONEXIT_H
