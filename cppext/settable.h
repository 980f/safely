#ifndef SETTABLE_H
#define SETTABLE_H

#include "changemonitored.h"
#include "argset.h"


/** persistance manager class for clumps of numbers, but without the actual persistance mechanism. 
This does some of the packing and unpacking of numerical objects but doesn't actually transport them.*/
class Settable :public ChangeMonitored {
private:
  /** due to the variants of assigning let's make the user be explicit  (@see copy vs @see changed)*/
  void operator =(int)=delete;
protected:
  /** this allows for copying from nominally different types, hence somewhat hidden from novice users. */
  virtual void operator =(const Settable &other){//no type cast?
    copy(other);
  }
public:
  virtual int numParams()const=0;
  virtual bool setParams(ArgSet&args)=0;
  virtual void getParams(ArgSet&args)const =0 ;

public:
  /** @returns whether posting should be requested, clears modified flags on both entities. */
  bool blockCheck(Settable &desired)const;
  /** compare */
  bool differs(Settable const &other)const;//compare
  /** assign. can inspect change flags or @see differed() */
  void copy(Settable const &other);
  /**assign, report if assignment caused something to change.*/
  bool changed(Settable const &other);
  //formally required to allow use in heaped systems:
  virtual ~Settable();
};

#include "cheaptricks.h" //changed<>()
/** Wrap any scalar type with settability. You gain the equivalent of Watched as a side effect. */
template <typename Scalar> class Setting: public Settable {
public:
  Scalar value;
  operator Scalar() const{
    return value;
  }

  Scalar operator =(Scalar other){
    also( ::changed<Scalar,Scalar>(value,other));
    return other;
  }

  int numParams()const {
    return 1;
  }

  bool setParams(ArgSet&args){
    return also(::changed(value,args.next(0)));
  }

  void getParams(ArgSet&args)const {
    args.next()=value;
  }

};


#endif // SETTABLE_H
