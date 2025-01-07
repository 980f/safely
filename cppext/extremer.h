#pragma once  //Copyright 2018 Andy Heilveil (github/980f)

/** classes for finding minima and maxima in a push environment (as data arrives in event driven system), 
 * versus the typical pull environment (a for loop iterating over data that is all present)
 *
 * There are two variants, one which records location information, one which does not.
 *
 * Not yet using <=> due to AVR compiler.
 */


/** negatory is whether we are seeking smallest else largest
 * preferLatter exists for use by Extremer class, so that it can share more code with this one even though the concept is only slightly interesting in this class.
 * If preferlatter is true then the compare includes '==' as a reason to update the value.
 */
template<typename Scalar, bool negatory , bool preferLatter > class SimpleExtremer {
public: //for convenience.
  /** whether any data has been inspected (else extremum is not valid)
   * This is easier than trying to come up with a type independent initial value of extremum.
  */
  bool started = false;

public: //could make this read-only
  Scalar extremum=Scalar();

public:
  /** @returns whether the extremum was updated */
  bool inspect(Scalar value) {
    if (started) {
      //if preferLatter we want '=' to fall through and return true, not return false
      if constexpr (negatory) {
        if constexpr (preferLatter) {
          if (extremum <= value) {
            return false;
          }
        } else if (extremum < value) {
          return false;
        }
      } else {
        if constexpr (preferLatter) {
          if (value <= extremum) {
            return false;
          }
        } else if (value < extremum) {
          return false;
        }
      }
    } else {
      started = true;
    }
    extremum = value;
    return true;
  } // inspect

  /** to reuse */
  void reset() {
    started = false;
    extremum = Scalar(); //for debug
  }
}; // class Extremer

/** negatory is whether we are seeking smallest else largest
 * preferLatter is how to deal with ties between new value and reigning champion, whether to have the latest one take the crown.
 */
template<typename Scalar, bool negatory , bool preferLatter > class Extremer : public SimpleExtremer<Scalar, negatory, preferLatter> {
  using Simple = SimpleExtremer<Scalar, negatory, preferLatter>; //compiler would NOT recognize the base class references without this aid.
public: //could make these read-only
  unsigned location = ~0; //default for debug
public:
  /** @returns whether the extremum or location was updated */
  bool inspect(unsigned loc, const Scalar &value) {
    if (Simple::inspect(value)) {
      location = loc;
      return true;
    } else {
      return false;
    }
  } // inspect

  void reset() {
    location = ~0;
    Simple::reset();
  }
}; // class Extremer

//prebuild the common ones:
class MaxDoubleFinder : public Extremer<double, false, false> {};

class MinDoubleFinder : public Extremer<double, true, false> {};

class MaxDouble : public SimpleExtremer<double, false, false> {};

class MinDouble : public SimpleExtremer<double, true, false> {};

//add more wrapped specializations here.
