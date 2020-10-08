#include "linear.h"
#include "minimath.h"

Linear::Linear() {
  trivialize();
}

Linear::Linear(double offset, double slope) {
  init(offset, slope);
}

void Linear::trivialize() {
  a[0] = 0.0;
  a[1] = 1.0;
}

int Linear::effectiveDegree() const {
  if (a[1] != 0) {
    return 1;
  }
  if (a[0] != 0) {
    return 0;
  }
  return -1;
}

/** inverse function, only definitely exists for degree 1*/
double Linear::x(double y) const {
  return ratio(y - a[0], a[1]);
}

bool Linear::setParams(ConstArgSet &args) {
  set(a[0], args);
  set(a[1], args);
  return isModified();
}

void Linear::getParams(ArgSet &args) const {
  args.next() = a[0];
  args.next() = a[1];
}

double &Linear::operator[](int which) {
  return a[which];
}

double Linear::p(int which) const {
  return a[which];
}

/** @return value of derivative of order @param prime evalued at @param x
    * default value of prime gives poly(x);*/
double Linear::y(double x, unsigned prime) const {
  switch (prime) {
  case 0:
    return a[0] + a[1] * x;
  case 1:
    return a[1];
  default:
    return 0;
  }
}

Linear &Linear::init(double offset, double slope) {
  a[0] = offset;
  a[1] = slope;
  return *this;
}
