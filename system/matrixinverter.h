#ifndef MATRIXINVERTER_H
#define MATRIXINVERTER_H


#include <vector>
/** dynamic sized multiregression. See class determinator for simple fixed size problems. */
class MatrixInverter {
public:
  using Gater = std::vector<bool> ;
  using Column = std::vector<double>;
  class Matrix: public std::vector<Column> {
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
 IE you can load up all the sample data then just by playing with these flags you can change the 'equation' */
  Gater ignore; //unused column&row, you can build the xXx matrix once and then iterate through different model options through modifying just these.
  /** @param size is used to presize arrays and matrices. */
  MatrixInverter(unsigned size);
public:
  //Identity matrix as a function call.
  static inline double diag(unsigned cl,unsigned rw){
    return (cl==rw)?1.0:0.0;
  }
  /**set xs to identity, gater's to all enabled */
  void clear();
  double compute();
  /** @param please  is an additional gating of the spew, see a static bool within the module for the other one.*/
  void dump(bool please);
  bool test();
private:
  inline bool inPlay(unsigned rw,unsigned focus) { return rw!=focus && !ignore[rw];}
};

#endif // MATRIXINVERTER_H
