#ifndef LINEARSTATISTIC_H
#define LINEARSTATISTIC_H
#include "minimath.h"

/**
  *  RMS averager
  */
template <class Numeric> class LinearStatistic{
  friend class Correlation<Numeric>;
protected:
  Numeric sum;
  Numeric sumSquares;
  int count;
public:
  LinearStatistic(){
    reset();
  }

  /** forget any data */
  void reset(){
    count = 0;
    sum = 0;
    sumSquares = 0;
  }

  /** include another datum into the set being evaluated, without actually storing it.*/
  void insert(Numeric datum){
    ++count;
    sum += datum;
    sumSquares += squared(datum);
  }

  /** removes a value, presuming that it was once supplied via insert */
  void remove(Numeric datum){
    if(count) {
      --count;
      sum -= datum;
      sumSquares -= squared(datum);
    }
  }

  /** replace one value with another, such as when doing a moving fit to a series of values  */
  void replace(Numeric older, Numeric newer){
    sum += newer - older;
    sumSquares += squared(newer) - squared(older);
  }

  /** @returns a common subexpression of many other computations */
  Numeric Lform() const {
    return count * sumSquares - squared(sum);
  }

  /** @returns the average */
  double average() const {
    return ratio(sum, count);
  }

  /** @returns the square of the variance
  *  todo:2 verify implementation!
   */
  double varsquared() const {
    return ratio(Lform(), count);
  }
};

/**
  * a linear correlation
  */
template <class Numeric> class Correlation {
public:
  LinearStatistic <Numeric> xx;
  LinearStatistic <Numeric> yy;
  Numeric sumCross;

  void insert(Numeric x, Numeric y){
    xx.insert(x);
    yy.insert(y);
    sumCross += x * y;
  }

  /** @returns a common subexpression */
  Numeric Lcross() const {
    return xx.count * sumCross - xx.sum * yy.sum;
  }

  /** @returns the cross correlation of its data pairs. Depending on your definition this might be the square of what you expect this name to be providing. */
  double crossCorrelation() const {
    return ratio(squared(Lcross()), xx.Lform() * yy.Lform());
  }
};

#endif // LINEARSTATISTIC_H
