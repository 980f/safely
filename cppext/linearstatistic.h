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

  void reset(void){
    count = 0;
    sum = 0;
    sumSquares = 0;
  }

  void insert(Numeric datum){
    ++count;
    sum += datum;
    sumSquares += squared(datum);
  }

  void remove(Numeric datum){
    if(count) {
      --count;
      sum -= datum;
      sumSquares -= squared(datum);
    }
  }

  void replace(Numeric older, Numeric newer){
    sum += newer - older;
    sumSquares += squared(newer) - squared(older);
  }

  Numeric Lform(void){
    return count * sumSquares - squared(sum);
  }

  double average(void){
    return ratio(sum, count);
  }

  double varsquared(void){ //todo:2 verify!
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

  Numeric Lcross(void){
    return xx.count * sumCross - xx.sum * yy.sum;
  }

  double crossCorrelation(void){
    return ratio(squared(Lcross()), xx.Lform() * yy.Lform());
  }
};

#endif // LINEARSTATISTIC_H
