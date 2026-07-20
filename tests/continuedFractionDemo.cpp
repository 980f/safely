#include <iostream>
#include <cmath>

#include "continuedfractionratiogenerator.h"
#include "cheaptricks.h"

using namespace std;

int testCFRG(int which) {
  static const double m[] = {M_PI,M_E,M_SQRT2,M_2_SQRTPI,M_LN10,M_LN2,M_LOG2E};
  if (unsigned(which) < countof(m)) {
    ContinuedFractionRatioGenerator cg;
    double arg = m[which];
    cout << arg << " was input"
      << ", math will be done using " << ContinuedFractionRatioGenerator::maxWorkingBits << " bits" << endl
      << " epsilon is:" << ContinuedFractionRatioGenerator::epsilon << endl;

    cg.restart(arg);
    for (unsigned step = 0; cg.step() && step++ < ContinuedFractionRatioGenerator::maxWorkingBits;) {
      auto error = cg.better() - arg;
      cout << step
      << "\t" << cg.numerator() << "/" << cg.denominator()
      << "\terror:" << error
      << "\t%err:" << 100.0*error/arg
      <<"\t9's:" << -log10(fabs(error)/arg)
      << endl;
    }
    return 0;
  }
  return -which;
}
