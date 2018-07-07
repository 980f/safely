#ifndef DETERMINATOR_H
#define DETERMINATOR_H

#include "matrixinverter.h"

/** solve a set of linear regressions.
  repackaged a simple determinator thingy in order to reuse the data block for multiple fits.

  NOTE VERY WELL: if the full equation doesn't have an offset term (correlate to '1') then the statistic measures herein are invalid.
  For convenience that offset term must be index 0. Having an offset in the correlates does not mean that it must be enabled for fitting, it just must be present.
*/
class LLSQcomputer : public MatrixInverter {
public:
  unsigned numSamples; //kept for degree of freedom calculation.
  double sumy2; //kept for stats
  double sumx(unsigned which);

  unsigned numParams()const;
  /**  N*sum x[i]^2 - sq(sum xp[])*/
  double Lxx(int which);
  double Lxy(int which);
  /** N * sum(Y squared) - square (sum Y) */
  double Lyy();
  /** std of y data*/
  double varY();
  double Rsquared(int which);
  double crossCorr(int i,int j);
  double multiRsquared();

public:
  Column ys; //sum x's*Y
public:
  Column solution;
  /** @returns df()+1 if all goes well, <df on failures */
  int numFit;

  LLSQcomputer(unsigned numCoeff);

  void clear();
  /**arrange for the given row and columns to not affect the others*/
  void applyIgnorance();
public:
  void include(double Y,const Column &correlates);
  /** @returns the number of successful fits, should == size*/
  int compute();
private:
  void fillinTriangle();
};

#endif // DETERMINATOR_H
