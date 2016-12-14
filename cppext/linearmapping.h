#ifndef LINEARMAPPING_H
#define LINEARMAPPING_H
#include "interval.h"
#include "settable.h" //argset

/**intended for perfect cases such as analog to digital conversion.
see LinearFit for calibration uses.*/
class LinearMapping:public Settable {
public:
  LinearMapping();
public:
  //and since these are public we can't cache anything.
  Interval xaxis;
  Interval yaxis;
public:
  LinearMapping& operator = (const LinearMapping &other);
  /** x clamped then converted to y */
  double y(double x) const ;
  /** y clamped then converted to x */
  double x(double y) const ;
  double yraw(double x) const ;
  double xraw(double y) const ;
  bool seemsTrivial() const ;
  void init(float ymax, float ymin, float xmax, float xmin);

  int numParams()const{
    return 2*xaxis.numParams();
  }
/** @returns whether settings are changed.*/
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;

};

#endif // LINEARMAPPING_H
