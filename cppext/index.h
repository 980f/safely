#ifndef INDEX_H
#define INDEX_H

#include "cheaptricks.h"
/** unsigned is used for all index operations.
 * For prior uses of int typically the only negative index value is a marker, -1.
 * It happens that if you view -1 as an unsigned it is the maximum possible value. That has the advantage of replacing the signed integer dance:
 * index>=0 && index < quantity with a simple index<quantity, less runtime code.
 *
 * The only risk here is that someone might use -1 as a quantity value indicating that there is not even a container to have a quantity of items in. Just don't do that, return a quantity of 0 for 'not a valid question', that will almost always yield the expected behavior.
*/
/** the magic value, it is all ones */
constexpr unsigned BadIndex=~0U;
//marker for bad difference of indexes:
constexpr unsigned BadLength=~0U; //legacy

/** we default indexes to ~0, and return that as 'invalid index'*/
inline bool isValid(unsigned index){
  return index!=BadIndex;
}

/** marker class for an integer used as an array index or related value. Leave all in header file as we need to strongly urge the compiler to inline all this code  */
struct Index {
  Index(unsigned raw=BadIndex):raw(raw){}

  unsigned raw;

  bool isOdd() const noexcept {
    return isValid()&& (raw&1);//lsb === is odd.
  }

  unsigned operator =(unsigned ord) noexcept {
    return raw=ord;
  }

  operator unsigned () const noexcept {
    return raw;
  }

  bool isValid() const noexcept {
    return raw!=BadIndex;//replace this with ~raw!=0 if compiler can't figure that out on its own.  Or load/inc/ inspect Z bit of status.
  }

  void clear() noexcept {
    raw=BadIndex;
  }

  /** @returns whether this is less than @param limit */
  bool in( const Index &limit) const noexcept {
    return raw<limit.raw;
  }

  /** @returns whether this contains @param other */
  bool has(const Index &other) const noexcept {
    return raw>other.raw;
  }

  /** maydo: convert negatives to canonical ~0*/
  unsigned operator += (unsigned other) noexcept {
    return raw+=other;
  }

  /** decrement IF valid */
  unsigned operator -= (unsigned other) noexcept {
    if(isValid()){
      return raw-=other;
    } else {
      return BadIndex;
    }
  }

  /** maydo: convert negatives to canonical ~0*/
  unsigned operator ++ (int) noexcept {
    return raw++;
  }
  /** maydo: convert negatives to canonical ~0*/
  unsigned operator ++ () noexcept {
    return ++raw;
  }

  /** @returns present value, then sets it to zero. */
  unsigned take() noexcept {
    return take(raw);
  }

  /** @returns up to @param howMany if present value is greater than that, else returns present value. Value is post-decremented by howMany but doesn't wrap.
   * If howMany is BadIndex then this returns BadIndex and doesn't change. <- If we drop that rule BadIndex is 'take all', for which functionality there is a specific method.
   * EG: if presently ==5 and you take 3 this will return 3 and become ==2. if presently ==1 and you take 3 this returns 1 and becomes 0.
*/
  unsigned take(unsigned howMany) noexcept {
    if(howMany<raw){
      raw-=howMany;
      return howMany;
    }
    if(howMany==BadIndex){
      return BadIndex;
      //and make no change.
    }
    return take(raw);
  }



};

#endif // INDEX_H
