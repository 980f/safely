#ifndef MATRIXINVERTER_H
#define MATRIXINVERTER_H


#include <vector>
/** dynamic sized multiregression. See determinator for simple fixed size problems */
class MatrixInverter {
public:
  typedef std::vector<bool> Gater;
  typedef std::vector<double> Column;
  class Matrix:public std::vector<Column> {
  public:
    Matrix(int size);
  };
public:
  const int size; //num X's == num Coeffs.
public:
  /** the presence of the bogus Cxy term precludes doing various elements in parallel via a matrix of Y's, also we exclude different samples per element meaning we'd need to reload the matrix anyway.*/
  Matrix xs; //sum xXx
  Matrix inv; //decon coeffs.
  /** for fit variants manipulate the ignores*/
  Gater ignore; //unused column&row, decons unfortunately has to always process all of them due to this being applied symmetrically.
  MatrixInverter(int size);
public:
  static inline double diag(int cl,int rw){
    return (cl==rw)?1.0:0.0;
  }
  /**set xs to identity, gater's to all enabled*/
  void clear();
  double compute();
  /** @ param please  is an additional gating of the spew, see a static bool within the module for the other one.*/
  void dump(bool please);
  bool test();
};
#define forSize(si) for(int si=size;si-->0;)


#endif // MATRIXINVERTER_H
