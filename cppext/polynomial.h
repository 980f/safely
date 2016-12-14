#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "settable.h"

/** @returns the value of a polynomial given by coefficients @param a,degree at point @param x. @param prime is the degree of differentiation to apply to the polynomial, only implemented for small values. e.g. for prime==1 the coefficients are effectively shifted once andmultiplied by their index  */
double polysum(double x,const double *a,int degree,unsigned prime=0);
/** given an array of coefficients of a polynomial return the ordinal of the highest non-zero one*/
int effectiveDegree(const double *a,int degree);

// malloc'ing version  /** 
//*/
//class Polynomial:public Settable {
//public:
//  const int degree;
//protected:
//  double *a;//[degree + 1];
//public:
//  Polynomial(int degree);
//  ~Polynomial();

//  /** @returns whether the assignment changed this object.*/
//  bool operator =(const Polynomial &other);
//  bool validIndex(int which)const;
//  double &operator[](int which)const;

//  int numParams()const{
//    return degree+1;
//  }
//  /** set coefficients. this is NOT a python list comprehension sort of thing.
//argument list is 0 to higher order*/
//  bool setParams(ArgSet&args);
//  /** export coefficients*/
//  void getParams(ArgSet&args)const;
//  /** @return value of derivative of order @param prime evalued at @param x
//    * default value of prime gives poly(x);*/
//  double y(double x, unsigned prime = 0)const;
//  int effectiveDegree()const;
//  double inv(double y)const;
//};


template <int degree>
class Polynomial:public Settable {
  
protected:
  double a[degree + 1];
public:
  Polynomial(){
    for(int ai = 0;ai <= degree;++ai) {
      a[ai]=0;
    }
  }
  ~Polynomial(){}  //the compiler made me do it
  
  /** @returns whether the assignment changed this object.*/
  
  bool operator =(const Polynomial<degree>&other){
    for(int ai = 0;ai <= degree;++ai) {
      set(a[ai], ai<=other.degree?other[ai]:0);
    }
    return isModified();
  }
  
  bool validIndex(int which)const{
    return which>=0 && which <=degree;
  }
  //  const double &operator[](int which)const{
  //    if(validIndex(which)){
  //      return a[which];
  //    } else {
  //      return a[0];
  //    }
  //  }
  
  double &operator[](int which){
    if(validIndex(which)){
      return a[which];
    } else {
      return a[0];
    }
  }
  
  int numParams()const{
    return degree+1;
  }
  /** set coefficients. this is NOT a python list comprehension sort of thing.
argument list is 0 to higher order*/
  bool setParams(ArgSet&args){
    for(int ai = 0;ai <= degree;++ai) {
      set(a[ai], args.next(0.0));
    }
    return isModified();
  } 
  /** export coefficients*/
  void getParams(ArgSet&args)const{
    for(int ai = 0; ai <= degree; ++ai) {
      args.next() = a[ai];
    }
  }
  
  /** @return value of derivative of order @param prime evaluated at @param x
    * default value of prime gives poly(x);  @see polysum */
  double y(double x, unsigned prime=0)const{
    return polysum(x,a,degree,prime);
  }
    
  int effectiveDegree()const{
    return ::effectiveDegree(a,degree);
  }
  
  double inv(double y)const{
    switch(effectiveDegree()){
      case -1: return 0.0;//null polynomial
      case 0: return a[0];
      case 1: return ratio(y-a[0],a[1]);
      case 2: //todo:3 greatest magnitude quadratic root with same sign as y
        //maydo: Newton Rhapson inversion
      default: return y;
        
    }
  }
};



#endif // POLYNOMIAL_H
