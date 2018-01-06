#ifndef POLYFILTER_H  //(C) 2017 Andrew L.Heilveil.
#define POLYFILTER_H

#include "centeredslice.h"
#include "extremer.h"
/** polynomial filter, first implementations use Savitsky-Golay algorithm as optimized by Andy Heilveil */
class PolyFilter {
protected:
  PolyFilter(unsigned hw);
public:
  virtual ~PolyFilter() = default;
  const int hw;

  virtual double slope() const = 0;
  virtual int signA1() const = 0;
  virtual double amplitude() const = 0;
  virtual void init(const CenteredSlice &slice) = 0;
  virtual void step(CenteredSlice &slice) = 0;

  struct Interpolation {
    //add interpolation components to refine location of characteristic
    int delta = 0; //running estimate, scaled by something to make it an integer
    int slope = 0; //denominator, deferring divide until final value encountered
    double tweak(){
      return ratio(double(delta),double(slope));
    }
  };

  template<bool rising> struct Inflection : public Extremer<int,rising,true> {
    Interpolation tweaker;
    /** the stored location is relative to some external point @param offset*/
    double absolute(int offset){
      return offset + Extremer<int,rising,true>::location - tweaker.tweak();
    }

  };

  struct ScanReport {
    Inflection<true> low;
    Inflection<false> high;
    Inflection<true> top;
  };

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
   *   @param report is fed obtuse data on a found peak, code fragment to make use of it is in comments in quadfilter.cpp */
  virtual bool scan(const CenteredSlice &slice, ScanReport &report) = 0;

}; // class PolyFilter

#endif // POLYFILTER_H
