#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/**
The items below with 'atomisable' in their description are snippets where an atomic test-and-change operation is expected.
As of the initial implementation such atomicity was never needed so we didn't actually apply <atomic> which wasn't around in 2012 when this pool of code was started.
*/

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

  /** same as operator bool, might remove that code and make people type the extra -- */
  bool operator--(int) noexcept;

  bool done() const noexcept{
    return counter==0;
  }

/** sometimes you go back one */
  unsigned operator ++() noexcept{
    return ++counter;
  }

  /** test, decrements if not already zero and @returns whether it just became zero */
  bool last() noexcept;

  bool hasNext() const noexcept {
    return counter>0;
  }

  /** just how weird can c++ be?
   * @returns a reference to the element of the @param array associated with this count value.
   * Note then when counting is done this will get you the 0th item repeatedly, not an exception. */
  template <typename Many> Many& next(Many array[]) const {
    return array[counter];
  }
};

/** @returns whether assigning @param newvalue to @param target changes the latter. the compare is for nearly @param bits, not an exact number. If nearly the same then
 * the assignment does not occur.
 *  This is handy when converting a value to ascii and back, it tells you whether that was significantly corrupting.
 */
bool changed(double&target, double newvalue,int bits = 32);

/** atomisable compare and assign
 * @returns whether assigning @param newvalue to @param target changes the latter */
template<typename Scalar1, typename Scalar2=Scalar1> bool changed(Scalar1 &target, const Scalar2 &newvalue){
  if(target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** atomisable test-and-clear */
template<typename Scalar> Scalar flagged(Scalar&varb) ISRISH; //mark as needing critical optimization

template<typename Scalar> Scalar flagged(Scalar&varb){
/** this implementation isn't actually atomic, we managed to single thread our whole codebase. */
  Scalar was = varb;
  varb = 0;
  return was;
}

/** atomisable test and set
 * if arg is false set it to true and return true else return false.*/
inline bool notAlready(bool &varb){
  if(!varb) {
    varb = true;
    return true;
  } else {
    return false;
  }
}

#endif // CHEAPTRICKS_H
