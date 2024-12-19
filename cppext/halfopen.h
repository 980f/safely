#pragma once // "(C) Andrew L. Heilveil, 2017"

/** a simple range class, simpler than Range and optimized for first use of HalfOpen(arrayIndex) */

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

  /** @returns the quantity to covered */
  Integrish span() const {
    return ordered()? highest-lowest : 0;
  }

/** @returns whether the span is devoid of content. */
  bool empty() const {
    return span()==0;
  }

  /**@returns whether the span has something in it (converse of empty) */
  operator bool() const {
    return ! empty();
  }
  /** @returns whether the two bounds are in natural order. virtual to allow checking for validity of each element */
  virtual bool ordered() const {
    return highest>=lowest;
  }
};

#include "index.h"
struct Span: HalfOpen<Index> {
  Span(Index low,Index high);
  Span();
   ~Span() override =default;

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

  bool nonTrivial() const noexcept;
  /** intersection */
  static Span overlap(const Span &one,const Span&other);

  /** make it @param more bigger. @returns if this makes span become semi-valid */
  bool stretchUp(unsigned more=1);
};
