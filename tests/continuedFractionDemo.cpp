#include <iostream>
#include <math.h>

#include "continuedfractionratiogenerator.h"
using namespace std;

int testCFRG(int which) {
  ContinuedFractionRatioGenerator cg;
  double arg = M_PI;
  switch (which) {
  case 0:
    arg = M_PI;
    break;
  case 1:
    arg = M_E;
    break;
  case 2:
    arg = M_SQRT2;
    break;
  default:
    arg = std::numeric_limits<double>::quiet_NaN();;
    break;
  }
  cout << arg << " was input"
    << ", math will be done using " << ContinuedFractionRatioGenerator::maxWorkingBits << " bits" << endl
    << " epsilon is:" << ContinuedFractionRatioGenerator::epsilon << endl;

  cg.restart(arg);
  for (unsigned step = 0; cg.step() && step++ < ContinuedFractionRatioGenerator::maxWorkingBits;) {
    cout << step << " " << cg.numerator() << "/" << cg.denominator() << " error:" << cg.better() - arg << endl;
  }
  return 0;
}
