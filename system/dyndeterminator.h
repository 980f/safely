#ifndef DETERMINATOR_H
#define DETERMINATOR_H

#include "matrixinverter.h"

/** solve a set of linear regressions.

  NOTE VERY WELL: if the full equation doesn't have an offset term (correlate to '1') then the statistic measures herein are invalid.
  For convenience that offset term must be index 0. Having an offset in the correlates does not mean that it must be enabled for fitting, it just must be present.
*/
class LLSQcomputer : public MatrixInverter {
public:
  unsigned numSamples; //kept for degree of freedom calculation.
  /** sum of Y^2 (givens). Retained for stats */
  double sumy2;
  /** sum of a correlate */
  double sumx(unsigned which);
  /** number of */
  unsigned numParams()const;
  /**  N*sum x[i]^2 - sq(sum xp[])*/
  double Lxx(unsigned which);
  double Lxy(unsigned which);
  /** N * sum(Y squared) - square (sum Y) */
  double Lyy();
  /** std of y data*/
  double varY();
  double Rsquared(unsigned which);
  double crossCorr(unsigned i,unsigned j);
  double multiRsquared();

public:
  Column ys;
public:
  Column solution;
  /** @returns df()+1 if all goes well, <df on failures */
  unsigned numFit;

  LLSQcomputer(unsigned numCoeff);

  void clear();
  /**arrange for the given row and columns to not affect the others*/
  void applyIgnorance();
public:
  /** @returns whether sample was actually included, it won't be if it has any nan's in it. */
  bool include(double Y,const Column &correlates);
  /** @returns the number of successful fits, should == size*/
  unsigned compute();
private:
  void fillinTriangle();
};

#endif // DETERMINATOR_H
