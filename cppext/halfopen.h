#ifndef HALFOPEN_H
#define HALFOPEN_H

/** a simple range class */

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

  /** quantity to operate upon */
  Integrish span() const {
    return highest-lowest;
  }

};

struct Span: public HalfOpen<unsigned> {
  Span(unsigned low,unsigned high):HalfOpen(low,high){}
};

#endif // HALFOPEN_H
