#ifndef HALFOPEN_H
#define HALFOPEN_H "(C) Andrew L. Heilveil, 2017"

/** a simple range class, simpler than Range and optimized for first use of HalfOpen(arrayIndex)
 * IE it is intended for walking a view across an array with no overlaps.
 * At the moment some stupid cases are not handled well.
 */

template <typename Integrish> struct HalfOpen {
  Integrish lowest;
  Integrish highest;
public:
  HalfOpen(Integrish lowest, Integrish highest):lowest(lowest),highest(highest){
    //#nada
  }

  HalfOpen(){
    lowest=highest=0; //all Integrish types have a zero.
  }

  virtual ~HalfOpen()=default;

  /** quantity to operate upon */
  Integrish span() const {
    return ordered()? highest-lowest : 0;
  }

  /** @returns whether the two bounds are in natural order. virtual to allow checking for validity of each element */
  virtual bool ordered() const {
    return highest>=lowest;
  }

  bool empty() const {
    return span()==0;
  }

  /** syntactic convenience for while loops */
  operator bool() const {
    return ! empty();
  }

};

#include "index.h"
struct Span: HalfOpen<Index> {
  Span(Index low,Index high);
  Span();
//  virtual ~Span();
  bool ordered() const override;
  /** move span to next possible one. default of 1 is for cutting out single character separators */
  void leapfrog(unsigned skip=1);
  /** set both ends to 'invalid'*/
  void clear();
  /** subtract a number presumed to be less than the lowest from both that and the highest */
  void shift(unsigned offset);
  /** take values from other, clear() other */
  void take(Span &other);
  /** @returns whether span lowest is reasonable but highest inValid, started but not completed */
  bool started() const noexcept;
  /** @returns whether the span has a non-zero length, which includes testing for valid endpoints. */
  bool nonTrivial() const noexcept;
  /** intersection */
  static Span overlap(const Span &one,const Span &other);
  /** make it @param more bigger. @returns if this makes span become semi-valid.
   * if neither bound is set you get 0..more.
   * if lower is valid but upper is not you get lower to more, which is often a bad idea and perhaps this function should be redefined!
   * if lower and upper are valid you get lower to upper+more.
   */
  bool stretchUp(unsigned more=1);
};

#endif // HALFOPEN_H
