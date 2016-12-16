#ifndef POLYFILTER_H
#define POLYFILTER_H

#include "centeredslice.h"

/** polynomial filter, first implemenations use Savitsky-Golay algorithm as optimized by Andy Heilveil */
class PolyFilter {
protected:
  PolyFilter(unsigned hw);
public:
  const int hw;
  struct Inflection {
    int location;
    int maxrmin; //discriminant for "biggest" whatever determination.

    int estimate;//running estimate, scaled by something to make it an integer
    int delta; //running estimate, scaled by something to make it an integer

    Inflection();
    /** the stored location is relative to some external point @param offset*/
    double absolute(int offset);
    /** use return to reduce cost of computing the tweak */
    bool recordif(bool changeit,int newvalue,int newlocation);
    /** use return to reduce cost of computing the tweak */
    bool morePositive(int newvalue,int newlocation);
    bool moreNegative(int newvalue,int newlocation);
  };

  virtual double slope() const = 0;
  virtual int signA1() const = 0;
  virtual double amplitude() const = 0;
  virtual void init(const CenteredSlice &slice) = 0;
  virtual void step(CenteredSlice &slice) = 0;

  struct ScanReport {
    bool meaningful;
    Inflection low;
    Inflection high;
    Inflection top;
  };

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
   *   @param report is fed obtuse data on a found peak, code fragment to make use of it is in comments in quadfilter.cpp */
  virtual void scan(const CenteredSlice &slice, ScanReport &report) = 0;

}; // class PolyFilter

#endif // POLYFILTER_H
