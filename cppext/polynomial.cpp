#include <string.h>
#include "minimath.h"
#include "polynomial.h"

unsigned Pnr(unsigned n, unsigned r) {
  unsigned product = n;
  while (--r) {
    product *= --n;
  }
  return product;
}

double polysum(double x, const double *a, unsigned degree, unsigned primes) {
  if (primes == 0) { //expedite most used case
    double acc = a[degree];
    for (unsigned i = degree; i-- > 0;) {
      acc *= x;
      acc += a[i];
    }
    return acc;
  }
  if (primes > degree) { //differentiated into oblivion
    return 0;
  }
  //the number of iterations will be degree-primes+1, although the +1 is done outside the actual loop
  //the a[] index will be degree .. prime descending
  double acc = a[degree] * Pnr(degree, primes);
  for (unsigned i = degree; i-- > primes;) {
    acc *= x;
    acc += a[i] * Pnr(i, primes);//note: Pnr(n-1,r) = (n-r) * (Pnr(n,r)/n), and that division never has a remainder.
  }
  return acc;
}

unsigned effectiveDegree(const double *a, unsigned degree) {
  do {
    if (a[degree] != 0) {
      break;
    }
  } while (--degree > 0);
  return degree;
}
