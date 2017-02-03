#include <string.h>
#include "minimath.h"
#include "polynomial.h"

double polysum(double x,const double *a,int degree, unsigned prime){
  double acc = a[degree];
  if(prime==0){//expedite most used case
    for(unsigned i = degree; i-->0; ) {
      acc *= x;
      acc += a[i];
    }
  } else {
    acc*=Cnr(degree,prime);
    for(unsigned i = degree; i-- > prime; ) {
      acc *= x;
      acc += a[i] * Cnr(i,prime);
    }
  }
  return acc;
}

int effectiveDegree(const double *a,int degree){
  for(int eff=degree;eff>=0;--eff){
    if(a[eff]!=0){
      return eff;
    }
  }
  return -1;//indicates poly will always return 0;
}


//Polynomial::Polynomial(int d):
//  degree(d),
//  a(StaticBuffer(double,degree + 1)) {  //deleted in destructor
//  memset(a, 0, sizeof(double[degree + 1]));
//}

//Polynomial::~Polynomial(){
//  StaticReleaseArray(a);
//}



//bool Polynomial::validIndex(int which)const{
//  return which>=0 && which <=degree;
//}

//double &Polynomial::operator[](int which)const{


///** set coefficients. this is NOT a python list comprehension sort of thing.
//argument list is 0 to higher order*/
//bool Polynomial::setParams(ArgSet&args)/* apply */

///** export coefficients*/
//void Polynomial::getParams(ArgSet&args)const

///** @return value of derivative of order @param prime evalued at @param x
//  * default value of prime gives poly(x);*/
//double Polynomial::y(double x, unsigned prime)const{
//  return polysum(x,a,degree,prime);
//}

//int Polynomial::effectiveDegree()const{
//  return ::effectiveDegree(a,degree);
//}

//double Polynomial::inv(double y)const

////////////////

