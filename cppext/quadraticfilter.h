#ifndef QUADRATICFILTER_H
#define QUADRATICFILTER_H

#include "linearfilter.h" //haven't extracted the common base class PolyFilter into its own files

class QuadraticFilter : public PolyFilter {
  //cached dependents on hw:
  const int hw2;//hw*hw
  const double S0;
  const int R0;//hw*(hw+1)
  const double S2;
  const int R2;// S4/S2
  const double S4;
  const double D4;

  //filter state memory:
  int Y[3];
  //for interpolations
  int delta[3];
  //criteria memory
  int est[3];
public:
  QuadraticFilter(unsigned hw);
  /** actual quadratic term at present point */
  double curvature()const;
  /** integer proportional to curvature() */
  int curvish()const;
  /** actual linear term at present point: curvature* 3 * (D4/S0) */
  double slope()const;
  int signA1()const ;
  /** smoothed amplitude*/	
  double amplitude()const;
  /** @returns integer proportional to amplitude at this point. suitable for relative amplitude reasoning.
	amplitude() * 5 * (D4*S4/S2) */
  int ampEstimate() const;

  void recordInflection(Inflection &flect) const;

  void init(const CenteredSlice &slice);
  void step(CenteredSlice &slice);

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
      @param peak records the most interesting points in the range
      @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
  bool scan(const CenteredSlice &slice,PeakFind &peak,int offset);

};

#endif // QUADRATICFILTER_H
