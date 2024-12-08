#pragma once

#if SAFELY_settable  //the settable interface is used to mate to non-volatile storage or other ascii based transport.
#include "settable.h"
#endif

/** @returns the value of a polynomial given by coefficients @param a,degree at point @param x.
* @param prime is the degree of differentiation to apply to the polynomial, only implemented for small values. e.g. for prime==1 the coefficients are effectively shifted once and multiplied by their index
*/
double polysum(double x, const double *a, unsigned degree, unsigned primes = 0);

/** given an array of coefficients of a polynomial return the ordinal of the highest non-zero one, ~0 if null polynomial */
unsigned effectiveDegree(const double *a, unsigned degree);

template<unsigned degree> class Polynomial{
protected:
  double a[degree + 1];

public:
  Polynomial() {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      a[ai] = 0;
    }
  }

  Polynomial(const double args[degree + 1]) {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      a[ai] = args[ai];
    }
  }

  template<typename  ... Args>
  Polynomial(const Args... args): Polynomial{} {
    const int incomingsize=sizeof...(args);
    double asarray[incomingsize] = {args ...};
    for (unsigned ai = 0; ai <= degree; ++ai) {
      a[ai] = ai <incomingsize  ? asarray[ai] : 0;
    }
  }

  ~Polynomial() = default; //the compiler made me do it

  /** @returns whether the assignment changed this object.
  * //todo:1 template a different degree for 2nd argument.
  */
  template<unsigned otherdegree>
  Polynomial& operator =(const Polynomial<otherdegree> &other) {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      a[ai]= ai <= otherdegree? other[ai] : 0;
    }
    return *this;
  }

  bool validIndex(unsigned which) const {
    return which <= degree;
  }

  double &operator[](unsigned which) {
    if (validIndex(which)) {
      return a[which];
    } else {
      return a[0];
    }
  }

  /** @return value of derivative of order @param prime evaluated at @param x
    * default value of prime gives poly(x);  @see polysum */
  double y(double x, unsigned prime = 0) const {
    return polysum(x, a, degree, prime);
  }

  unsigned effectiveDegree() const {
    return ::effectiveDegree(a, degree);
  }

  double inv(double y) const {
    switch (effectiveDegree()) {
      case ~0:
        return 0.0; //null polynomial
      case 0:
        return a[0]; //actually should return any x we wish to. This is a cheat ysed to snag the offset
      case 1:
        return ratio(y - a[0], a[1]);
      case 2: //todo:3 greatest magnitude quadratic root with same sign as y
        //maydo: Newton Rhapson inversion
      default:
        return y;
    }
  }
};
