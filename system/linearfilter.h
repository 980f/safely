#ifndef LINEARFILTER_H
#define LINEARFILTER_H

#include "centeredslice.h"
#include "storable.h" //todo:2 try to remove storable from here, or create a microcontroller compatible one.
#include "storednumeric.h"

struct PeakSearch:public Stored {
  StoredReal energy;//0== don't use, stale entry in table
  StoredReal width;//0== don't use in resolution calibration
  StoredInt  which;//spectrum
  PeakSearch (Storable &node);
  bool isValid()const;
  bool usableWidth()const;
};

struct PeakFind: public Stored {
  StoredReal center;
  StoredReal amplitude; //at center
  StoredReal low;    //channel of greatest positive slope
  StoredReal riser;  //most positive slope
  StoredReal high;   //channel of greatest negative slope
  StoredReal faller; //most negative slope
  StoredInt si;  //spectrum backlink, for convenience.
  StoredBoolean efit;//used in fit
  StoredBoolean rfit;
  PeakFind (Storable &node);
  void clear();
  double width()const;
  bool hasWidth()const;
};


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

  virtual double slope()const=0;
  virtual int signA1()const =0;
  virtual double amplitude()const =0;
  virtual void init(const CenteredSlice &slice)=0;
  virtual void step(CenteredSlice &slice)=0;

  /** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
      @param peak records the most interesting points in the range
      @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
  virtual bool scan(const CenteredSlice &slice,PeakFind &peak,int offset)=0;

};


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
