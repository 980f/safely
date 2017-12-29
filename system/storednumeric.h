#ifndef STOREDNUMERIC_H
#define STOREDNUMERIC_H

#include "stored.h"
#include "sigcuser.h"
//////////////////////////////
template< typename Numeric > class StoredNumeric: public Stored {
public:
  StoredNumeric(Storable &node,Numeric fallback=Numeric(0)): Stored(node) {
    node.convertToNumber(detail<Numeric>());
    setDefault(fallback);
  }

  operator Numeric() const {
    return node.getNumber< Numeric >();
  }

  /**if not already assigned then assign to given value */
  Numeric setDefault(Numeric def) {
    return node.setDefault(def);
  }

  Numeric assign(Numeric newnum) {
    return node.setNumber< Numeric >(newnum);
  }

  /** try to make sense of @param value as a number and if it seems to be one set this to that value, else no change.
 @returns the actual value of the node, which may not be the value passed in. */
  Numeric setFromText(Cstr value){
    return assign(value.cvt<Numeric>(native()));
  }

  Numeric operator = (Numeric newnum) {
    return assign(newnum);
  }

  Numeric operator -=(Numeric newnum) {
    return operator =(node.getNumber< double >() - newnum);
  }

  Numeric operator +=(Numeric newnum) {
    return operator =(node.getNumber< double >() + newnum);
  }

  Numeric operator *=(Numeric newnum) {
    return operator =(node.getNumber< double >() * newnum);
  }

  //added for sake of StoredInt.
  int operator ++() {
    return int(operator += (1));
  }

  int operator --() {
    return int(operator -= (1));
  }

  /** copy value from node of exactly the same type */
  Numeric operator =(const StoredNumeric< Numeric > &other) {
    return assign(other.node.Storable::template getNumber< Numeric >());
  }

  /** be no higher than argument: set to minimum of self and rhs */
  bool depress(Numeric rhs){
    Numeric temp=this->native();
    if(::depress(temp,rhs)){
      assign(temp);
      return true;
    } else {
      return false;
    }
  }

  /** be no lower than argument: set to maximum of self and rhs */
  bool elevate(Numeric rhs){
    Numeric temp=this->native();
    if(::elevate(temp,rhs)){
      assign(temp);
      return true;
    } else {
      return false;
    }
  }

  /** useful in var-args list, instead of a cast.*/
  Numeric native()const {
    return node.getNumber< Numeric >();
  }

  /** @return whether this item was just updated from the @param parent storage*/
  bool legacy(Storable *parent,const char *name){
    if(parent){
      if(Storable *oldnode=parent->existingChild(name)){
        assign(oldnode->getNumber<Numeric>());
        parent->removeChild(*oldnode);
        return true;
      }
    }
    return false;
  }
  /** configuration upgrade, @params are for previous storage of this value */
  bool legacy(Storable &parent,const char *name){
    return legacy(&parent,name);
  }

  /** @return a functor that when invoked will set this object's value to what is passed at this time.*/
  SimpleSlot setLater(Numeric value) {
    return sigc::hide_return(sigc::bind(sigc::mem_fun(this, &StoredNumeric< Numeric >::assign),value));
  }

  /** a slot that will set the value of this from a double (regardless of element's actual type)*/
  sigc::slot<void, Numeric> setter(){
    return sigc::hide_return( sigc::bind( mem_fun(node, &Storable::setNumber<Numeric>), Storable::Edited));
  }

  /** @return a functor that when invoked will get this object's current value.*/
  sigc::slot<Numeric> getLater(){
    return MyHandler(StoredNumeric<Numeric>::native);
  }

  /** @return a functor that when invoked will set this object's value to what is returned by the given @param functor invoked at that time.*/
  SimpleSlot assignLater(sigc::slot<Numeric,void> functor){
    return sigc::hide_return(sigc::compose(MyHandler(StoredNumeric<Numeric>::assign),functor));
  }

  /** useful for sending value changes */
  SimpleSlot applyTo(sigc::slot<void,Numeric> functor){
    return sigc::compose(functor,getLater());
  }

  /** hook up to send changes to the @param given functor, and if @param kickme call that functor now*/
  sigc::connection sendChanges(sigc::slot<void,Numeric> functor,bool kickme=false){
    return onAnyChange(applyTo(functor),kickme);
  }

  /** hook up to send changes to a simple variable. THIS IS DANGEROUS as you must manage the @return sigc::connection and disconnect it when the target gets deallocated. */
  sigc::connection onChangeUpdate(Numeric &target,bool kickme=false){
    if(kickme){
      target=native();
    }
    return onAnyChange(sigc::bind(&assignTo<Numeric>,sigc::ref(target),getLater()));
  }

  /** lamda for simple assignment. todo: replace with C++ lamda once we figure out how to make those work with StoredGroup */
  static void Assign(StoredNumeric<Numeric> &item, Numeric value){
    item.assign(value);
  }
};

typedef StoredNumeric<double> StoredReal;
typedef StoredNumeric<int> StoredInt;
typedef StoredNumeric<unsigned> StoredCount;

#if cppGetsFixedOnDerivingFromTemplate //presently have to republish the whole interface
struct StoredBoolean:public StoredNumeric<bool>{
  StoredBoolean(Storable &node,bool fallback=false);

  sigc::connection whenSet(SimpleSlot action, bool kickme=false);
  sigc::connection whenCleared(SimpleSlot action, bool kickme=false);

};
#else

//todo: arrange to tolerate 'true' and 'false' text on underlying node. I think an enumerizer will do it.
typedef StoredNumeric<bool> StoredBoolean;
/** filtering adaptors on change signal */
sigc::connection whenSet(StoredBoolean &thing,SimpleSlot action);
sigc::connection whenCleared(StoredBoolean &thing,SimpleSlot action);
#endif

/** lamda for fixing up a group which contains an integer index into some other group*/
void adjustIndex(unsigned removed, StoredCount &index);



#endif // STOREDNUMERIC_H
