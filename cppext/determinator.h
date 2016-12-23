#ifndef DETERMINATOR_H
#define DETERMINATOR_H


/** compute determinant of a subset of a matrix.
 *  This api has some candy to make it easy to use for regression analysis, specifically it provides for passing the 'X' matrix and 'Y' vector and then computing the
 * determinant with an option to replace one of the X columns with the Y column. This allows one to build the regression matrix and Y data and do subsets of the
 * possible regressions without shuffling that data around into smaller matrices.
 *  This was first implemented for use in a not particularly large microcontroller.
 */


/** not template portion of Determinator*/
class DeterminatorCore {
  int size;
  bool *rower;
  bool *columner;
  bool *ignorer;

  double *Y;
  double **X;
  /** these are members for debug, could be locals to descend. */
  int which; //where on the diagonal
  bool yish; //whether we are processing a numerator of a Cramer's rule solution.

  DeterminatorCore(
    int size,
    bool *rower,
    bool *columner,
    bool *ignorer,
    double ys[], double **xs);
  /** which column might be replaced with Y data, and whether to do so,
   * @returns determinant of submatrix */
  double descend(void);
public:
  double compute(int which, bool yish);
}; // class DeterminatorCore

/** this template exists to coordinate allocating related structures */
template<int MaxProblem = 4> //maximum matrix size, defaulted to the smallest that isn't hard coded as pre-computed case.
class Determinator : private DeterminatorCore {
  typedef bool Gater[MaxProblem];
  typedef double Column[MaxProblem];

  Gater&ignorer; //caller can clip the problem
  Gater rower;   //tracks progress in processing rows
  Gater columner;//tracks progress in processing columns

//  double *Y;
//  Column *X;
public:
  Determinator(Gater&ignore, double ys[], Column xs[]) :
    DeterminatorCore(MaxProblem,rower,columner,ignore,ys,xs){
    //this class exists to allocate the right sized data and then pass it all to the core.
  }

}; // class Determinator


#if 0
An older compiler allowed a template whose parameter was a value to have functions defined out of line (in a cpp file).
The internal implementation was of a const field member of the class.
Sadly that was fixed and now I have to do a bunch of monkeying to explicitly implement that.
#endif


#endif // DETERMINATOR_H
