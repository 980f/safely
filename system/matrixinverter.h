#ifndef MATRIXINVERTER_H
#define MATRIXINVERTER_H


#include <vector>
/** dynamic sized multiregression. See class determinator for simple fixed size problems. */
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
  Matrix xs; //sum xXx
  Matrix inv; //
  /** for fit variants manipulate these ignores */
  Gater ignore; //unused column&row, you can build the xXx matrix once and then iterate through different model options through modifying just these.
  /** @param size is used to presize arrays and matrices. */
  MatrixInverter(int size);
public:
  static inline double diag(int cl,int rw){
    return (cl==rw)?1.0:0.0;
  }
  /**set xs to identity, gater's to all enabled*/
  void clear();
  double compute();
  /** @param please  is an additional gating of the spew, see a static bool within the module for the other one.*/
  void dump(bool please);
  bool test();
private:
  inline bool inPlay(int rw,int focus) { return rw!=focus && !ignore[rw];}
};

#endif // MATRIXINVERTER_H
