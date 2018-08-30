#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/**
 *  The items below with 'atomisable' in their description are snippets where an atomic test-and-change operation is expected.
 *  As of the initial implementation such atomicity was never needed so we didn't actually apply <atomic> which wasn't around in 2012 when this pool of code was
 * started.
 */



/** atomisable compare and assign
 * @returns whether assigning @param newvalue to @param target changes the latter */
template<typename Scalar1, typename Scalar2 = Scalar1> bool changed(Scalar1 &target, const Scalar2 &newvalue){
  //attempt to cast newvalue to Scalar1 via declaring a local Scalar1 here wasn't universally acceptible, tried to implicitly construct and that can be expensive
  if(target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** @returns whether assigning @param newvalue to @param target changes the latter. the compare is for nearly @param bits, not an exact number. If nearly the same then
 * the assignment does not occur.
 *  This is handy when converting a value to ascii and back, it tells you whether that was significantly corrupting.
 */
bool changed(double&target, double newvalue,int bits = 32);


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

/** atomisable test-and-clear take a value, @returns @param varb's value then clears it.*/
template<typename Scalar> Scalar take(Scalar&varb){//replaces all but boolean use of flagged.
  return postAssign(varb,Scalar(0));
}

/** given pointer to a pointer to something that was dynamically allocated delete that thing and null the pointer. This gets more consistent segfaults on use-after-free */
template<typename Scalar> void Free(Scalar **p2p){
  if(p2p) {
    delete *p2p;
    *p2p = nullptr;
  }
}

/** originally was the same code as the newer 'take' but since the code was found duplicated in another file the name wasn't quote right.
 * 'flagged' is still a very good name for the boolean implementation of take, so we rework it thusly:
 */
bool flagged(bool &varb) ISRISH; //mark as needing critical optimization
inline bool flagged(bool &varb){
  return take(varb);
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
