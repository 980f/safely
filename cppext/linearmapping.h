#ifndef LINEARMAPPING_H
#define LINEARMAPPING_H
#include "interval.h"
#include "settable.h" //argset

/** bilinear mapping, with transport handle.
 * intended for perfect cases such as analog to digital conversion.
 *  see LinearFit for gauge calibration uses.*/
class LinearMapping : public Settable {
public:
  LinearMapping();
public:
  //and since these are public we can't cache anything.
  Interval xaxis;
  Interval yaxis;
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
///////////// Settable ///////////////
  unsigned numParams() const  override{
    return 2 * xaxis.numParams();
  }

/** @returns whether settings are changed.*/
  bool setParams(ConstArgSet&args) override;
  void getParams(ArgSet&args) const override;

}; // class LinearMapping

#endif // LINEARMAPPING_H
