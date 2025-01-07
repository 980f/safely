#pragma once

/** compute determinant of a subset of a matrix.
 *  This api has some candy to make it easy to use for regression analysis, specifically it provides for passing the 'X' matrix and 'Y' vector and then computing the
 * determinant with an option to replace one of the X columns with the Y column. This allows one to build the regression matrix and Y data and do subsets of the
 * possible regressions without shuffling that data around into smaller matrices.
 *  This was first implemented for use in a not particularly large microcontroller.
 */


/** not template portion of Determinator*/
class DeterminatorCore {
  unsigned size;
  bool *rower;
  bool *columner;
  bool *ignorer;

  double *Y;
  double **X;
  /** these are members for debug, could be locals to 'descend'. */
  unsigned which; //where on the diagonal
  bool yish; //whether we are processing a numerator of a Cramer's rule solution.

  DeterminatorCore(
    unsigned size,
    bool *rower,
    bool *columner,
    bool *ignorer,
    double ys[], double **xs);
  double descend();
public:

  /** @param which is which column might be replaced with Y data, and @param yish is whether to do so,
   * @returns determinant of submatrix */
  double compute(unsigned which, bool yish);
}; // class DeterminatorCore

/** this template exists to coordinate allocating related structures */
template<int MaxProblem = 4> //maximum matrix size, defaulted to the smallest that isn't hard coded as pre-computed case.
class Determinator : protected DeterminatorCore {
  using Gater = bool [MaxProblem];
  using Column = double[MaxProblem];

  Gater rower;   //tracks progress in processing rows
  Gater columner;//tracks progress in processing columns
public:
  Determinator(Gater&ignore, Column ys, Column xs[]) :
    DeterminatorCore(MaxProblem,rower,columner,ignore,ys,xs){
    //this class exists to allocate the right sized data and then pass it all to the core.
  }

}; // class Determinator
