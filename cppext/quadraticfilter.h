#ifndef QUADRATICFILTER_H
#define QUADRATICFILTER_H

#include "polyfilter.h"

class QuadraticFilter : public PolyFilter {
  //cached dependents on hw:
  const unsigned hw2;//hw*hw
<<<<<<< HEAD
  const double S0;
=======
  const double S0;//double used to extend range, and since this is only used to compute a double.
>>>>>>> dp5qcu
  const unsigned R0;//hw*(hw+1)
  const double S2;
  const unsigned R2;// S4/S2
  const double S4;
  const double D4;

  //filter state memory:
  int Y[3];
  //for interpolations
  int delta[3];
  //criteria memory
  int est[3];
public:
  explicit QuadraticFilter(unsigned hw);
  /** actual quadratic term at present point */
  double curvature() const;
  /** integer proportional to curvature() */
  int curvish() const;

  /** actual linear term at present point: curvature* 3 * (D4/S0) */
  double slope() const override;
  double slope(int extremum) const ;

  int signA1() const override;
  /** smoothed amplitude*/
  double amplitude() const override;
  double amplitude(int datum) const;
  /** @returns integer proportional to amplitude at this point. suitable for relative amplitude reasoning.
   *  amplitude() * 5 * (D4*S4/S2) */
  int ampEstimate() const;

  void init(const CenteredSlice &slice) override;
  void step(CenteredSlice &slice) override;

<<<<<<< HEAD
  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
      @param peak records the most interesting points in the range */
  bool scan(const CenteredSlice &slice, ScanReport &report) override;
=======
protected:
  void recordInflection(PolyFilter::Inflection &flect) const;
  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side
   * @param report records the most interesting points in the range*/
  void scan(const CenteredSlice &slice,ScanReport &report) override;
>>>>>>> dp5qcu

protected:
  void recordInflection(PolyFilter::Interpolation &flect) const;
}; // class QuadraticFilter

#endif // QUADRATICFILTER_H
