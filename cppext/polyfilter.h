#pragma once //(C) 2017,2024 Andrew L.Heilveil.  AKA github/980f.

#include "centeredslice.h"

/** polynomial filter, base class with support for tracking filter progress and dataset interesting parts*/
class PolyFilter {
protected:
  /** @param hw is half width of data span, full width is 2*hw+1. Odd width is forced due to significant performance gains */
  PolyFilter(unsigned hw);
  virtual ~PolyFilter()=default;
public:
  const unsigned hw;
  struct Datum {
    int amplitude = 0; //discriminant for "biggest" whatever determination.
    int location = 0; //not unsigned as this is an abstract user coordinate, not necessarily an array index.
    Datum() = default;
  };
  /** places where derivatives are zero or maximum is often the main goal in filtering. */
  struct Inflection {
    Datum point;
    int estimate;//running estimate, scaled by something to make it an integer
    int delta; //running estimate, scaled by something to make it an integer

    Inflection();
    /** the stored location is relative to some external point @param offset*/
    double absolute(int offset) const;

    /** use return to reduce cost of computing the tweak
    @returns whether the inflection was updated */
    bool morePositive(const Datum &testpoint);
    bool moreNegative(const Datum &testpoint);
  protected:
    /** @returns @param changeit,  code common to morePositive and moreNegative, use them. */
    bool recordif(bool changeit, const Datum &testpoint);

  };

  virtual double slope() const = 0;
  virtual int signA1() const = 0;
  virtual double amplitude() const = 0;
  virtual void init(const CenteredSlice &slice) = 0;
  virtual void step(CenteredSlice &slice) = 0;

  struct ScanReport {
    bool meaningful = false;
    Inflection low;
    Inflection high;
    Inflection top;
  };

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
   *   @param report is fed obtuse data on a found peak, code fragment to make use of it is in comments in quadfilter.cpp */
  virtual void scan(const CenteredSlice &slice, ScanReport &report) = 0;

}; // class PolyFilter
