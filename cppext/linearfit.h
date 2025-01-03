#pragma once

#include "realstatistic.h"
#include "polynomial.h"
using Linear = Polynomial<2>;
/** adds generation of a formula for a line to RealCorrelation's fitting.
 * This is not part of RealCorrelation so as to not force the polynomial class upon users. */

class LinearFit:public RealCorrelation {
public:
  LinearFit()=default;
  Linear equation() const {//#Linear's are cheap enough to copy, hence no '&' in the return.
    return {offset(), slope()};
  }
};
