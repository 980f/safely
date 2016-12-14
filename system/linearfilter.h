#ifndef LINEARFILTER_H
#define LINEARFILTER_H



/** a polynomial fit to integer indexed data */
class LinearFilter: public PolyFilter {
  //cached dependents on hw:
  const double S0;
  const double S2;
  //filter state memory:
  int Y[2];
  //for interpolations
  int delta[2];
public:
  LinearFilter(unsigned hw);
  double slope()const;
  int signA1()const ;
  double amplitude()const;
  void init(const CenteredSlice &slice);
  void step(CenteredSlice &slice);

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
      @param peak records the most interesting points in the range
      @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
  bool scan(const CenteredSlice &slice,PeakFind &peak,int offset);

};


#endif // LINEARFILTER_H
