#ifndef LINEARFILTER_H
#define LINEARFILTER_H

#include "polyfilter.h"

/** a polynomial fit to integer indexed data */
class LinearFilter : public PolyFilter {
  //cached dependents on hw:
  const double S0;
  const double S2;
  //filter state memory:
  int Y[2];
  //for interpolations
  int delta[2];
public:
  LinearFilter(unsigned hw);
  double slope() const override;
  int signA1() const override;
  double amplitude() const override;
  void init(const CenteredSlice &slice,bool fastly=true) override;//default fastly to true, false is for unit testing
  void step(CenteredSlice &slice) override;

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
   *   @param peak records the most interesting points in the range
   *   @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
  void scan(const CenteredSlice &slice,ScanReport &report) override;

}; // class LinearFilter


#endif // LINEARFILTER_H
