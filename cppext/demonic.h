#ifndef DEMONIC_H
#define DEMONIC_H

#include <functional> //todo: requires STLPort rowley package, need to fork that into a repo we control
#include "minimath.h" //depress and elevate

/** a numeric value that may have unlimited side effects when set.
*  Numeric should have an operator != Numeric, operator !=[0,1]
the side effect is provided to objects of this class via @see onChange().
*/


#ifdef _STLP_STD_NAME
using namespace _STLP_STD_NAME;
#endif

template<typename Numeric> class Demonic {
protected:
  Numeric item;

/** lambda will be called with (newvalue, oldvalue) */
  using DeltaFn = void(Numeric, Numeric);

/** lambda will be called with (newvalue) */
  using SetterFn =  void(Numeric);

/** lambda will be called, it must somehow know how to get the value(s) of interest. */
  using NotifierFn =  void(void);

  DeltaFn *demon;

public:
  /** default init for value, not coercing '0' */
  Demonic(void) : item() {
    //#nada
  }

/** explicit init for value */
  Demonic(const Numeric &other) : item(other) {
    //#nada
  }

/** this is NOT cumulative, only the most recent invocation's call will be invoked on a change.
It is theoretically possible to make a list via "andThen" wrapper around a pair of functors, but let us wait until we have a good use case to dig into that syntax.

The demon will be passed the PRIOR value of the object, you can inspect the new value by capturing the object in your lambda.
If @param kickme is true then the demon is called with its present value as the new one and the default value for the numeric type as the old.
  */
  void onAnyChange(DeltaFn &&ademon, bool kickme = false) {
    //todo:0 if present value of demon is nontrivial then set the new value to a demontuple link thingy, which invokes both members of the tuple
    demon = ademon;
    if (kickme) {
      ademon(item, Numeric());
    }
  }

/** lambda will be called with (newvalue) */
  void onAnyChange(SetterFn &&ademon, bool kickme = false) {
    onAnyChange([ademon](Numeric is, Numeric) {
      ademon(is);
    }, kickme);
  }

/** lambda will be called with (newvalue) */
  void onAnyChange(NotifierFn &&ademon, bool kickme = false) {
    onAnyChange([ademon](Numeric, Numeric) {
      ademon();
    }, kickme);
  }

  operator Numeric() const {
    return item;
  }

/** sometimes the compiler can't know to cast, like in varargs to a printf */
  Numeric native() const {
    return item;
  }

  Numeric set(const Numeric &newvalue) {
    if(demon){
      if (newvalue != item) {
        Numeric was = item;
        item = newvalue;
        demon(newvalue, was);
      }
      return item;
    } else {
      return item = newvalue;
    }
  }

  Numeric operator=(const Numeric &other) {
    return set(Numeric(other));
  }

  Numeric operator=(Numeric &&other) {
    return set(Numeric(other));
  }

  template<typename OtherNumeric> Numeric operator=(const OtherNumeric &other) {
    return set(Numeric(other));
  }

  template<typename OtherNumeric> Numeric operator=(OtherNumeric &&other) {
    return set(Numeric(other));
  }

  template<typename OtherNumeric> Numeric operator*=(const OtherNumeric &other) {
    if (other != 1) {
      return (set(item * Numeric(other)));
    } else {
      return item;
    }
  }

  template<typename OtherNumeric> Numeric operator+=(const OtherNumeric &other) {
    if (other != 0) {
      return (set(item + Numeric(other)));
    } else {
      return item;
    }
  }

  template<typename OtherNumeric> Numeric operator-=(const OtherNumeric &other) {
    if (other != 0) {
      return set(item - Numeric(other));
    } else {
      return item;
    }
  }

//added for sake of StoredInt.
  int operator++() {
    return int(operator+=(1));
  }

  int operator--() {
    return int(operator-=(1));
  }

  /** be no higher than argument: set to minimum of self and rhs */
  bool depress(Numeric rhs) {
    Numeric temp = this->native();
    if (::depress(temp, rhs)) {
      set(temp);
      return true;
    } else {
      return false;
    }
  }

  /** be no lower than argument: set to maximum of self and rhs */
  bool elevate(Numeric rhs) {
    Numeric temp = this->native();
    if (::elevate(temp, rhs)) {
      set(temp);
      return true;
    } else {
      return false;
    }
  }
};

#endif // DEMONIC_H
