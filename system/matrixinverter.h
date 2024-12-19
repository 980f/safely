#pragma once

#include <vector>

/** this is for dynamic sized multi-regression. See class determinator for fixed size problems. */
class MatrixInverter {
public:
  using Gater = std::vector<bool>;
  using Column = std::vector<double>;

  class Matrix : public std::vector<Column> {
  public:
    Matrix(unsigned size);
  };

public:
  const unsigned size;

  //a symmetric real matrix of the correlate cross products
  Matrix xs;
  //the inverse of xs
  Matrix inv;
  /** for fit variants manipulate these ignores.
   * IE you can load up all the sample data then just by playing with these flags you can change the 'equation'.
   * These mark the unused column&row.
  */
  Gater ignore;

  /** @param size is used to presize arrays and matrices. */
  MatrixInverter(unsigned size);

public:
  //Identity matrix as a function call.
  static double diag(unsigned cl, unsigned rw) {
    return (cl == rw) ? 1.0 : 0.0;
  }

  /**set xs to identity, gater's to all enabled */
  void clear();

  double compute();

  /** log the internal data.
   * @param please  is an additional gating of the spew, see a static bool within the module for the other one.*/
  void dump(bool please);

  /** multiplies the xs and the inv matrices, should get an Identity one.
   * As of this writing it is expected that you will use a debugger on the "return true" to inspect the result.
   */
  bool test();

private:
  bool inPlay(unsigned rw, unsigned focus) {
    return rw != focus && !ignore[rw];
  }
};
