#pragma once

#include "ranged.h"
#include "minimath.h"

#if SAFELY_settable  //the settable interface is used to mate to non-volatile storage or other ascii based transport.
#include "settable.h"
#endif

/** bilinear mapping, with transport handle.
 * intended for perfect cases such as analog to digital conversion.
 *  see LinearFit for gauge calibration uses.*/
class LinearMapping
#if SAFELY_settable  //the settable interface is used to mate to non-volatile storage or other ascii based transport.
  : public Settable
#endif
{
public:
  LinearMapping();
  using Axis = Ranged;
  //type Interval disappeared, this is a minimal reconstruction missing most of its idiot checking.
//  struct Axis:HalfOpen<double>{
//    void setto(double max, double min){
//      lowest=min;
//      highest=max;
//    }
//    bool nonTrivial() const {
//      return !isNan(lowest) && !isNan(highest) && highest!=lowest;
//    }
//    double start() const {
//      return lowest;
//    }
//    /** @returns width of defined region, negative for reversed definition */
//    double width() const {
//      return highest-lowest;
//    }
//    double fragment(double d)const {
//      return (d-lowest)/width();
//    }
//    double clamped(double d) const {
//      return d<lowest?lowest:d>highest?highest:d;
//    }
//  };
public:
  //and since these are public we can't cache anything.
  Axis xaxis;
  Axis yaxis;
public:
  LinearMapping& operator = (const LinearMapping &other);
  /** @returns x clamped then converted to y */
  double y(double x) const;
  /** @returns y clamped then converted to x */
  double x(double y) const;

  /** @returns y(x) ignoring bounds */
  double yraw(double x) const;
  /** @returns x(y) ignoring bounds */
  double xraw(double y) const;

  bool seemsTrivial() const;
  /** set for diagonal given by the parameters. */
  void init(float ymax, float ymin, float xmax, float xmin);

#if SAFELY_settable  //the settable interface is used to mate to non-volatile storage or other ascii based transport.
///////////// Settable ///////////////
  unsigned numParams() const  override{
    return 2 * xaxis.numParams();
  }

/** @returns whether settings are changed.*/
  bool setParams(ArgSet&args) override;
  void getParams(ArgSet&args) const override;
#endif
}; // class LinearMapping
