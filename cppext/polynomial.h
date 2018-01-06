#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "settable.h"

/** @returns the value of a polynomial given by coefficients @param a,degree at point @param x. @param prime is the degree of differentiation to apply to the polynomial, only implemented for small values. e.g. for prime==1 the coefficients are effectively shifted once andmultiplied by their index  */
double polysum(double x,const double *a,int degree,unsigned prime=0);
/** given an array of coefficients of a polynomial return the ordinal of the highest non-zero one, ~0 if null polynomial */
unsigned effectiveDegree(const double *a,unsigned degree);

template <unsigned degree> class Polynomial:public Settable {
<<<<<<< HEAD

=======
  
>>>>>>> dp5qcu
protected:
  double a[degree + 1];
public:
  Polynomial(){
    for(unsigned ai = 0;ai <= degree;++ai) {
      a[ai]=0;
    }
  }
<<<<<<< HEAD
  ~Polynomial(){}  //the compiler made me do it

=======
  ~Polynomial() = default;  //the compiler made me do it
  
>>>>>>> dp5qcu
  /** @returns whether the assignment changed this object.*/

  bool operator =(const Polynomial<degree>&other){
    for(unsigned ai = 0;ai <= degree;++ai) {
<<<<<<< HEAD
      set(a[ai], ai<=other.degree?other[ai]:0);
    }
    return isModified();
  }

  bool validIndex(unsigned which)const{
    return which <=degree;
  }

=======
      set(a[ai], ai<=degree?other[ai]:0);//todo:1 template a different degree for 2nd argument.
    }
    return isModified();
  }
  
  bool validIndex(unsigned which)const{
    return which <=degree;
  }
  
>>>>>>> dp5qcu
  double &operator[](unsigned which){
    if(validIndex(which)){
      return a[which];
    } else {
      return a[0];
    }
  }
<<<<<<< HEAD

=======
  
>>>>>>> dp5qcu
  unsigned numParams()const override{
    return degree+1;
  }
  /** set coefficients. this is NOT a python list comprehension sort of thing.
argument list is 0 to higher order*/
  bool setParams(ArgSet&args) override{
    for(unsigned ai = 0;ai <= degree;++ai) {
      set(a[ai], args.next(0.0));
    }
    return isModified();
  }
  /** export coefficients*/
  void getParams(ArgSet&args)const override{
    for(unsigned ai = 0; ai <= degree; ++ai) {
      args.next() = a[ai];
    }
  }

  /** @return value of derivative of order @param prime evaluated at @param x
    * default value of prime gives poly(x);  @see polysum */
  double y(double x, unsigned prime=0)const{
    return polysum(x,a,degree,prime);
  }
<<<<<<< HEAD

=======
    
>>>>>>> dp5qcu
  unsigned effectiveDegree()const{
    return ::effectiveDegree(a,degree);
  }

  double inv(double y)const{
    switch(effectiveDegree()){
      case ~0: return 0.0;//null polynomial
      case 0: return a[0];
      case 1: return ratio(y-a[0],a[1]);
      case 2: //todo:3 greatest magnitude quadratic root with same sign as y
        //maydo: Newton Rhapson inversion
      default: return y;

    }
  }
};



#endif // POLYNOMIAL_H
