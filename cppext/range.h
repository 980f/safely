#ifndef RANGE_H
#define RANGE_H

//minimath takes care of this, limits.h implemenations weren't standard enough: #include <limits.h>
#include "minimath.h"
#include "cheaptricks.h"

/**
 * integer types are half open: open on the high end
 */

//#define HASNAN (std::numeric_limits<PrimitiveNumeric>::has_quiet_NaN)

/**
 *  In various places you will have to add a :: in front of your Range declaration due to someone else (Gtk) having such a class inside some namespace .
 */
template<typename PrimitiveNumeric> class Range {
protected:
  //making these public requires that we do not cache any combination of them
  PrimitiveNumeric lowest;
  PrimitiveNumeric highest;
  bool reversed;
public:
  Range( ){
    trivialize();
  }

  Range( PrimitiveNumeric highest, PrimitiveNumeric lowest ){
    setto(highest,lowest);
  }

  bool isValid(void) const {
    return lowest <= highest;
  }

  bool nonTrivial(void) const {
    return lowest < highest;
  }

  void trivialize(){
//    if(HASNAN){
    setto(Nan,Nan);
//    } else {
//      setto(0,0);
//    }
  }

  /** @return whether given number is in HALF OPEN range.
   *   use cmp()==0 to get fully inclusive compare */
  bool contains(const PrimitiveNumeric other) const {
    return lowest <= other && other < highest;
  }

  /** exact compare*/
  bool operator == (const Range<PrimitiveNumeric> &other) const {
    return lowest == other.lowest && highest == other.highest;
  }

  Range<PrimitiveNumeric>& operator = (const Range<PrimitiveNumeric> &other){
    lowest = other.lowest;
    highest = other.highest;
    reversed = other.reversed;
    return *this;
  }

  bool canonicize(){
    if(flagged(reversed)) {
      myswap(lowest,highest);
      return true;
    } else {
      return false;
    }
  }

  /** @return -1: if @param d is below the range, +1 if above, 0 is inside CLOSED range*/
  int cmp(PrimitiveNumeric d) const {
    if(d < lowest) {
      return -1;
    }
    if(d > highest) {
      return 1;
    }
    return 0;
  }

  PrimitiveNumeric constrained(const PrimitiveNumeric &value) const {
    if(value < lowest) {
      return lowest;
    }
    if(value> highest) {
      return highest;
    }
    return value;
  }

  /** if outside then shift inside,  if wider then clip.
   *  maydo: add option to prefer keeping low vs high when we have to clip*/
  void adjustInto(const Range<PrimitiveNumeric>&bounder){
    // if left is lower pan up
    PrimitiveNumeric shiftlow = bounder.lowest - lowest;
    if(shiftlow>0) {
      shift(shiftlow);
      shiftlow = 0;
    }
    PrimitiveNumeric shifthigh = highest - bounder.highest;
    if(shifthigh>0) {
      if(-shiftlow>shifthigh) {
        //then we can purely shift
        shift(-shifthigh);
      } else {
        shift(shiftlow);
        highest = bounder.highest;
      }
    }
  } // adjustInto

  PrimitiveNumeric width(void) const {
    return highest - lowest;
  }

  PrimitiveNumeric center(void) const {
    return (highest + lowest) / 2;
  }

  /** @returns where in this range the @param d value is. */
  double fragment(PrimitiveNumeric d) const {
    return ratio(d - lowest, width());
  }

  /** @returns whether ends were swapped in order to make a valid range.*/
  virtual bool setto(const PrimitiveNumeric higher, const PrimitiveNumeric lower){
    reversed = lower > higher; //diagnostic
    this->highest = higher;
    this->lowest = lower;
    return true;
  } /* setto */

  PrimitiveNumeric start(void) const {
    return lowest;
  }

  PrimitiveNumeric end(void) const {
    return highest;
  }

  /** N.B.: the name is a bit more generic than the implemenation. You will have to read the code to make sure this implements what 'disJoin' means to you.
   *  Special case: if this contains other then returns false, else would have to split into two.*/
  bool disJoin(const Range<PrimitiveNumeric > &other ){
    bool disjoint = false;
    //todo:00 nan-proof
    if(highest<other.lowest) {
      disjoint = true;
    }

    if(lowest>other.highest) {
      disjoint = true;
    }

    if(!disjoint) {
      if(other.contains(lowest)) {
        lowest = other.highest;
      }
      if(other.contains(highest)) {
        highest = other.lowest;
      }
    }
    return nonTrivial();
  } // disJoin

  /** if overlap make one big happy window out of this, return false if no overlap*/
  bool merge(const Range<PrimitiveNumeric > &other ){
    if(isNan(highest)) {
      highest = other.highest;
    }
    if(isNan(lowest)) {
      lowest = other.lowest;
    }
    if(highest<other.lowest) {
      return false;
    }
    if(lowest>other.highest) {
      return false;
    }
    if(other.contains(lowest)) {
      lowest = other.lowest;
    }
    //even if above executes continue, to handle case of other totally containing this.
    if(other.contains(highest)) {
      highest = other.highest;
    }
    return true;
  } // merge

  /** modify this to enclose both itself and the @param other
   *  note that Nan limits will be set to 'other' */
  void enclose(const Range<PrimitiveNumeric > &other ){
    if(other.isValid()) {
      depress(lowest,other.lowest);
      elevate(highest,other.highest);
    }
  }

  /** modify this to enclose @param other.
   *  note that Nan limits will be set to 'other' */
  void enclose(PrimitiveNumeric other){
    depress(lowest,other);
    elevate(highest,other);
  }

  /** make this range not include anything that isn't also included in @param other.
   *  gives Nan,Nan if ranges don't overlap. */
  void overlap(const Range<PrimitiveNumeric > &other ){
    elevate(lowest,other.lowest);
    depress(highest,other.highest);
    if(!isValid()) {
      trivialize();
    }
  }

  /**expand range @param bias 1:stretch low side, -1: stretch high side, 0 keep center */
  void stretch(double factor,int bias){
    double newwidth(factor*width());
    switch(bias) {
    case 1://keep highest
      lowest = highest - newwidth;
      break;
    case 0://keep center
    {
      double midpoint = center();
      newwidth /= 2;
      lowest = midpoint - newwidth;
      highest = midpoint + newwidth;
    }
    break;
    case -1:
      highest = lowest + newwidth;
      break;
    } // switch
  } // stretch

  void shift(PrimitiveNumeric offset){
    highest += offset;
    lowest += offset;
  }

}; // class Range

/**
 * java-like range, highest is not part of the range
 */
class IndexRange : public ::Range<unsigned int> {
public:
  /***/
  int length(void) const {
    return highest - lowest;
  }

  int span(void) const {
    return length() + 1;
  }

  bool inRange(unsigned int cursor) const {
    return cursor < highest;
  }

}; // class IndexRange


#endif // RANGE_H
