#ifndef LINEAR_H
#define LINEAR_H

#include "settable.h"

/** linear mapping with persistence support.

todo:2 use new Nnomial class to replace this.
  Exists because the Polynomial class from firmware (/cppext/) leaks horribly on a real OS.*/
class Linear: public Settable {
  double a[2];
public:
  Linear();
  /** for initialized constants*/
  Linear(double offset,double slope);
  /** so that y() returns its argument*/
  void trivialize();
  /** -1 for always returns 0, 0 for returns a constant, 1 for does something interesting.*/
  int effectiveDegree()const;

  /** inverse function*/
  double x(double y)const ;

  unsigned numParams()const override {
    return 2;
  }
  bool setParams(ArgSet&args)override ;
  void getParams(ArgSet&args)const override ;
  /** access to individual coefficient*/
  double &operator[](int which);
  /** value of coefficient, note the const*/
  double p(int which)const;

  /** @return value of derivative of order @param prime evalued at @param x
    * default value of prime gives poly(x);*/
  double y(double x, unsigned prime = 0)const;
  double operator()(double x) const {
    return y(x);
  }
};

#endif // LINEAR_H
