#ifndef LINEARFIT_H
#define LINEARFIT_H

#include "realstatistic.h"
#include "linear.h"

class LinearFit:public RealCorrelation {
public:
  LinearFit();
  Linear equation();
  int N()const{
    return xx.N();
  }
  /** @returns whether stats are significant or if @param barely then or's whether A and B are significant */
  bool isMeaningful(bool barely)const;
};

#endif // LINEARFIT_H
