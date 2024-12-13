#pragma once

#include <functional>

/** non sigc version of "Watched" class.
 * Lacks sigc's ejection of demons who are freed.
 *
 * a numeric value that may have unlimited side effects when set.
*  The Numeric should have an operator != Numeric, operator !=[0,1]
the side effect is provided to objects of this class via @see onAnyChange().
*/

template<typename Numeric> class Demonic {
Numeric thing;

/** lambda will be called with (newvalue, oldvalue) */
using DeltaFn = std::function<void (Numeric,Numeric)>;

using SetterFn = const std::function<void (Numeric)>;

DeltaFn demon;

public:
  /** default init for value, not coercing '0' */
Demonic() : thing(){
   //#nada
}

/** explicit init for value */
Demonic(const Numeric &other) : thing(other){
  //#nada
}

/** this is NOT cumulative, only the most recent invocation's call will be invoked on a change.
It is theoretically possible to make a list via "andThen" wrapper around a pair of functors, but let us wait until we have a good use case to dig into that syntax.

The demon will be passed both the new and the PRIOR values of the object
If @param kickme is true then the demon is called with its present value as the new one and the default value for the numeric type as the old.
  */
void onAnyChange(DeltaFn&& ademon,bool kickme=false){
  demon = ademon;
  if(kickme){
    demon(thing,Numeric());
  }
}

/** lambda will be called with just (newvalue) */
void onAnyChange(SetterFn setterDemon,bool kickme=false){
  onAnyChange([setterDemon](Numeric is,Numeric ignored){
    setterDemon(is);
  },kickme);
}

/** mimics a scalar */
operator Numeric() const {
  return thing;
}

  /** core of all assignment operators */
Numeric set(const Numeric &newvalue){
  if (newvalue != thing) {
    Numeric was = thing;
    thing = newvalue;
    demon(newvalue, was);
  }
  return thing;
}

  /** assigning from another type*/
template<typename OtherNumeric> Numeric operator =(const OtherNumeric &other){
  return set(static_cast<Numeric>(other));
}

template<typename OtherNumeric> Numeric operator *=(const OtherNumeric &other){
  if (other != 1) {//4performance and debug
    return set(thing * static_cast<Numeric>(other));
  } else {
    return thing;
  }
}

template<typename OtherNumeric> Numeric operator +=(const OtherNumeric &other){
  if (other != 0) {//4performance and debug
    return set(thing + static_cast<Numeric>(other));
  } else {
    return thing;
  }
}

template<typename OtherNumeric> Numeric operator -=(const OtherNumeric &other){
  if (other != 0) {//4performance and debug
    return set(thing - static_cast<Numeric>(other));
  } else {
    return thing;
  }
}

}; // class Watched
