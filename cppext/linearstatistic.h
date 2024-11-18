#pragma once
/**
* (C) 2024 Andy Heilveil (github/980f) from copy-editing realstatistic.
The goal is to allow the math to stay integer as long as possible, compared to realstatistic which does everything in double.
*/
#include "minimath.h" //squared, ratio for divide by zero handling

/**
  *  RMS averager of pairs of numbers.
  */
template<typename Numeric> class Correlation; //must forward ref to friend a class which contains this one.
template<typename Numeric> class LinearStatistic {
  friend class Correlation<Numeric>;

protected:
  Numeric sum;
  Numeric sumSquares;
  unsigned count;

public:
  LinearStatistic() {
    reset();
  }

  void reset(void) {
    count = 0;
    sum = 0;
    sumSquares = 0;
  }

  void insert(Numeric datum) {
    ++count;
    sum += datum;
    sumSquares += squared(datum);
  }

  void remove(Numeric datum) {
    if (count) {
      --count;
      sum -= datum;
      sumSquares -= squared(datum);
    }
  }

  void replace(Numeric older, Numeric newer) {
    sum += newer - older;
    sumSquares += squared(newer) - squared(older);
  }

  double mean() const {
    return ratio(sum, count);
  }

  Numeric Lform() const {
    return count * sumSquares - squared(sum);
  }

  double varsquared() const { //todo:2 verify! (useful function but name might be wrong)
    return ratio(Lform(), count);
  }

  unsigned N() const {
    return count;
  }
};

/**
  * a linear correlation
  */
template<typename Numeric> class Correlation {
  LinearStatistic<Numeric> xx;
  LinearStatistic<Numeric> yy;
  Numeric sumCross;

public:
  Correlation() {
    reset();
  }

  void reset() {
    sumCross = 0;
    xx.reset();
    yy.reset();
  }

  void insert(Numeric x, Numeric y) {
    xx.insert(x);
    yy.insert(y);
    sumCross += x * y;
  }

  void remove(Numeric x, Numeric y) {
    xx.remove(x);
    yy.remove(y);
    sumCross -= x * y;
  }

  Numeric Lcross(const) {
    return xx.count * sumCross - xx.sum * yy.sum;
  }

  double crossCorrelation() const {
    return ratio(squared(Lcross()), xx.Lform() * yy.Lform());
  }

  double slope() const {
    return ratio(Lcross(), xx.Lform());
  }

  double offset() const {
    return yy.mean() - xx.mean() * slope();
  }

  double nines() const {
    double r2 = crossCorrelation();
    return r2 >= 1.0 ? Infinity : -log10(1 - r2); //test for 1.0 so we don't get NAN on perfect fits.
  }

  double ssd() const {
    return xx.sumSquares + yy.sumSquares - 2 * sumCross;
  }

  /** @returns square root of chi^2 divided by @param adj defaulted for linear fit */
  double var(unsigned adj = 2) const {
    int denom = xx.N() - adj;
    if (denom <= 0) {
      return Nan;
    }
    return sqrt(ratio(ssd(), denom));
  }
};
