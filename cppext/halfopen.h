#ifndef HALFOPEN_H
#define HALFOPEN_H

#include "safely.h"
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

  virtual ~HalfOpen(){}

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

};

struct Span: public HalfOpen<Index> {
  Span(Index low,Index high);
  Span();
  virtual ~Span();
  bool ordered() const override;
  /** move span to next possible one. default of 1 is for cutting out single character seperators */
  void leapfrog(unsigned skip=1);
  /** set both ends to 'invalid'*/
  void clear();
};

#endif // HALFOPEN_H
