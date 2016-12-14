#ifndef DETERMINATOR_H
#define DETERMINATOR_H


/** compute determinant of a subset of a matrix. 
 This api has some candy to make it easy to use for regression analysis, specifically it provides for passing the 'X' matrix and 'Y' vector and then computing the determinant with an option to replace one of the X columns with the Y column. This allows one to build the regression matrix and Y data and do subsets of the possible regressions without shuffling that data around into smaller matrices.
This was first implemented for use in a not particularly large microcontroller.
*/

template <int MaxProblem=4> //maximum matrix size, defaulted to the smallest that isn't hard coded as pre-computed case.
class Determinator {
  typedef bool Gater[MaxProblem];
  typedef double Column[MaxProblem];

  Gater&ignorer; //caller can clip the problem
  Gater rower;   //tracks progress in processing rows
  Gater columner;//tracks progress in processing columns

  double *Y;
  Column *X;
  /** the next two being members cause this class's instances to be single threaded*/
  int which; //where on the diagonal
  bool yish; //whether we are processing a numerator of a Cramer's rule solution.
public:
  Determinator(Gater & ignore, double ys[], Column xs[]);
  /** which column might be replaced with Y data, and whether to do so,
    * @returns determinant of submatrix */
  double compute(int which, bool yish);
protected:
  double descend(void);
};

#endif // DETERMINATOR_H
