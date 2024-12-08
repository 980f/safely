#pragma once
#if __has_include ("argset.h")
#include "argset.h" //should seperate this guy out.
#define haveArgsInterface 2
#endif
#include "cheaptricks.h" //flagged

/** manages a "dirty" bit, extended must use the set functions rather than assignment */
class ChangeMonitored {
protected:
  //the 'dirty' bit.
  bool modified;

public:
  ChangeMonitored() {
    modified = false; //#true caused things to start up in syntax controlled order rather than as commanded
  }

  virtual ~ChangeMonitored() = default;

  /** made overloadable so that compound objects can clear member flags as well, on a case by case basis.*/
  virtual bool wasModified() {
    return flagged(modified);
  }

  virtual bool isModified() const {
    return modified;
  }

  bool also(bool child) {
#if 1 //provides breakpoint on actual change
    if (!modified && child) {
      modified = true;
    }
    return modified;
#else
    return modified |= child;
#endif
  }

  bool also(const ChangeMonitored &other) {
    return also(other.modified);
  }

  /** added return for use in gui "actually changed" event generation.*/
  template<typename Scalar> bool set(Scalar &parm, Scalar value) {
    bool altered = changed(parm, value);
    also(altered);
    return altered;//#do NOT return what also returned, we only want to pass along whether the immediate assignment caused a change.
  }

#if haveArgsInterface //legacy stuff that will rise again, but with a better ArgSet implementation
  template<typename Scalar> void set(Scalar &parm, ArgSet &args) {
    if (args.hasNext()) {
      also(changed(parm, Scalar(args.next())));
    }
  }

  template<typename Scalar> void set(Scalar &parm, ArgSet &args, Scalar def) {
    also(changed(parm, args.next(def)));
  }

  void set(float &parm, ArgSet &args, double def = 0.0) {
    also(changed(parm, float(args.next(def))));
  }
#endif

}; // class ChangeMonitored

/** a numeric value that is marked as having been changed since the last time you asked about it.
 *  Numeric should have an operator != Numeric, operator !=[0,1] */
template<typename Numeric> class Watched : public ChangeMonitored {
  Numeric thing;

public:
  Watched() : thing() {}

  Watched(const Numeric &other) : thing(other) {
    //#nada
  }

  operator Numeric() const {
    return thing;
  }

  Numeric set(const Numeric &other) {
    modified = changed(thing, other);
    return thing;
  }

  template<typename OtherNumeric> Numeric operator =(const OtherNumeric &other) {
    return set(Numeric(other));
  }

  template<typename OtherNumeric> Numeric operator *=(const OtherNumeric &other) {
    if (other != 1) {
      return set(thing * Numeric(other));
    }
    return thing;
  }

  template<typename OtherNumeric> Numeric operator +=(const OtherNumeric &other) {
    if (other != 0) {
      return set(thing + Numeric(other));
    }
    return thing;
  }

  template<typename OtherNumeric> Numeric operator -=(const OtherNumeric &other) {
    if (other != 0) {
      return set(thing - Numeric(other));
    }
    return thing;
  }
}; // class Watched
