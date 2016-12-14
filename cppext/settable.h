#ifndef SETTABLE_H
#define SETTABLE_H

#include "changemonitored.h"
#include "argset.h"


/** persistance manager class for clumps of numbers, but without the actual persistance mechanism. 
This does some of the packing and unpacking of numerical objects but doesn't actually transport them.*/
class Settable :public ChangeMonitored {
private:
  void operator =(int){}
protected:
  virtual void operator =(const Settable &other){//no type cast?
    copy(other);
  }
public:
  virtual int numParams()const=0;
  virtual bool setParams(ArgSet&args)=0;
  virtual void getParams(ArgSet&args)const =0 ;

  /** @return whether posting should be requested, clears modified flags on both entities. */
  bool blockCheck(Settable &desired);
  /** compare */
  bool differs(Settable const &other)const;//compare
  /**assign. can inspect change flags or @see differed() */
  void copy(Settable const &other);
  /**assign, report if assignment caused something to change.*/
  bool differed(Settable const &other);

};

#endif // SETTABLE_H
