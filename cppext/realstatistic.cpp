#include "realstatistic.h"

#include "minimath.h" //divide by zero
#include <cmath> //sqrt, log

RealStatistic::RealStatistic() {
  reset();
}

void RealStatistic::reset() {
  count = 0;
  sum = 0;
  sumSquares = 0;
}

void RealStatistic::insert(double datum) {
  ++count;
  sum += datum;
  sumSquares += squared(datum);
}

void RealStatistic::remove(double datum) {
  if (count) {
    --count;
    sum -= datum;
    sumSquares -= squared(datum);
  }
}

void RealStatistic::replace(double older, double newer) {
  sum += newer - older;
  sumSquares += squared(newer) - squared(older);
}

double RealStatistic::Lform() const {
  return count * sumSquares - squared(sum);
}

double RealStatistic::mean() const {
  return ratio(sum, count);
}

double RealStatistic::ssd() const {
  if (count <= 0) {
    return Nan;
  }
  return ratio(Lform(), count);
}

double RealStatistic::sxd(unsigned used) const {
  int denom = count - used;
  if (denom <= 0) {
    return Nan;
  }
  return sqrt(ratio(ssd(), denom));
}

RealCorrelation::RealCorrelation() {
  reset();
}

void RealCorrelation::reset() {
  xx.reset();
  yy.reset();
  sumCross = 0;
}

void RealCorrelation::insert(double x, double y) {
  xx.insert(x);
  yy.insert(y);
  sumCross += x * y;
}

void RealCorrelation::remove(double x, double y) {
  xx.remove(x);
  yy.remove(y);
  sumCross -= x * y;
}

double RealCorrelation::Lcross() const {
  return xx.count * sumCross - xx.sum * yy.sum;
}

double RealCorrelation::crossCorrelation() const {
  return ratio(squared(Lcross()), xx.Lform() * yy.Lform());
}

double RealCorrelation::slope() const {
  return ratio(Lcross(), xx.Lform());
}

double RealCorrelation::offset() const {
  return yy.mean() - xx.mean() * slope();
}

double RealCorrelation::nines() const {
  double r2 = crossCorrelation();
  return r2 >= 1.0 ? Infinity : -log10(1 - r2); //test for 1.0 so we don't get NAN on perfect fits.
}

double RealCorrelation::ssd() const {
  return xx.sumSquares + yy.sumSquares - 2 * sumCross;
}

double RealCorrelation::var(unsigned adj) const {
  int denom = xx.N() - adj;
  if (denom <= 0) {
    return Nan;
  }
  return sqrt(ratio(ssd(), denom));
}
