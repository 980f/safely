#ifndef LINEARFIT_H
#define LINEARFIT_H

#include "realstatistic.h"
#include "polynomial.h"
using Linear = Polynomial<2>;
/** adds generation of a formula for a line to RealCorrelation's fitting. */

class LinearFit {
public:
  RealCorrelation correlation;
  LinearFit();
  Linear equation(){//#Linear's are cheap enough to copy, hence no '&' in the return.
    return Linear(correlation.offset(),correlation. slope());
  }
  unsigned N()const{
    return correlation.N();
  }
};

#endif // LINEARFIT_H
