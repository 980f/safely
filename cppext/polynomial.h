#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "settable.h"


/** @returns the value of a polynomial given by coefficients @param a,degree at point @param x. @param prime is the degree of differentiation to apply to the polynomial, only implemented for small values. e.g. for prime==1 the coefficients are effectively shifted once andmultiplied by their index  */
template<typename Floater=float> Floater polysum(Floater x, const Floater *a, int degree, unsigned prime = 0) {
  Floater acc = a[degree];
  if (prime == 0) {//expedite most used case
    for (unsigned i = degree; i-- > 0;) {
      acc *= x;
      acc += a[i];
    }
  } else {
    acc *= Cnr(degree, prime);
    for (unsigned i = degree; i-- > prime;) {
      acc *= x;
      acc += a[i] * Cnr(i, prime);
    }
  }
  return acc;
}

/** given an array of coefficients of a polynomial return the ordinal of the highest non-zero one, ~0 if null polynomial */
template<typename Floater=float> unsigned effectiveDegree(const Floater *a, unsigned degree) {
  for (int eff = degree; eff >= 0; --eff) {
    if (a[eff] != 0) {
      return eff;
    }
  }
  return BadIndex;//indicates poly will always return 0;
}


/** @tparam degree is highest power supported, not number of coefficients. */
template<unsigned degree, typename Floater=float> class Polynomial : public Settable {

protected:
  Floater a[degree + 1];
public:
  Polynomial() {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      a[ai] = 0;
    }
  }

  ~Polynomial() = default;  //the compiler made me do it

  /** @returns whether the assignment changed this object.*/

  template<typename OtherFloater=Floater>
  bool operator=(const Polynomial<degree, OtherFloater> &other) {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      set(a[ai], ai <= degree ? other[ai] : 0);//todo:1 template a different degree for 2nd argument.
    }
    return isModified();
  }

  bool validIndex(unsigned which) const {
    return which <= degree;
  }

  Floater &operator[](unsigned which) {
    if (validIndex(which)) {
      return a[which];
    } else {
      return a[0];
    }
  }

  unsigned numParams() const override {
    return degree + 1;
  }

  /** set coefficients. this is NOT a python list comprehension sort of thing.
argument list is 0 to higher order*/
  bool setParams(ConstArgSet &args) override {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      set(a[ai], args.next(0.0));
    }
    return isModified();
  }

  /** export coefficients*/
  void getParams(ArgSet &args) const override {
    for (unsigned ai = 0; ai <= degree; ++ai) {
      args.next() = a[ai];
    }
  }

  /** @return value of derivative of order @param prime evaluated at @param x
    * default value of prime gives poly(x);  @see polysum */
  Floater y(Floater x, unsigned prime = 0) const {
    return polysum(x, a, degree, prime);
  }

  unsigned effectiveDegree() const {
    return effectiveDegree(a, degree);
  }

  Floater inv(Floater y) const {
    switch (effectiveDegree()) {
      case ~0:
        return 0.0;//null polynomial
      case 0:
        return a[0];
      case 1:
        return ratio(y - a[0], a[1]);
      case 2: //todo:3 greatest magnitude quadratic root with same sign as y
        //maydo: Newton Rhapson inversion
      default:
        return y;
    }
  }
};

#endif // POLYNOMIAL_H
