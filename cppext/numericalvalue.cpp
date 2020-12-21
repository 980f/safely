#include "numericalvalue.h"
#include "logger.h"
#include "eztypes.h"
#include "minimath.h" //nan
#include "cheaptricks.h" //changed
#include "index.h" //for 'unsigned' reasoning



/** @returns value converted to bool */
template <> bool NumericalValue::cast<bool>() const noexcept{
  switch (is) {
  default:
  case Truthy:
    return storage.bee;
  case Whole:
    return storage.eye!=0;
  case Counting:
    return storage.ewe!=0;
  case Floating:
    return storage.dee!=0.0&&::isNormal(storage.dee);
  }
}

/** @returns value converted to double */
template <> double NumericalValue::cast<double>() const noexcept{
  switch (is) {
  case Truthy:
    return storage.bee?1.0:0;
  case Whole:
    return double(storage.eye);
  case Counting:
    return storage.ewe!=BadIndex?double(storage.ewe):Nan;
  default:
  case Floating:
    return storage.dee;
  }
}


/** @returns value saturatedly converted to unsigned */
template <> unsigned NumericalValue::cast<unsigned>() const noexcept{
  switch (is) {
  case Truthy:
    return storage.bee?1:0;
  case Whole://int to unsigned with saturation, all negatives are ~0
    return storage.eye>=0?unsigned(storage.eye):BadIndex;
  default:
  case Counting:
    return storage.ewe;
  case Floating://todo:1 find stdlib saturating conversion function.
    return ::isNormal(storage.dee)&&storage.dee>0.0&&storage.dee<double(BadIndex)?unsigned(storage.dee):BadIndex;
  }
}

/** @returns value saturatedly converted to int */
template <> int NumericalValue::cast<int>() const noexcept{
  switch (is) {
  case Truthy:
    return storage.bee?1:0;
  default:
  case Whole:
    return storage.eye;
  case Counting:
    return int(storage.ewe);
  case Floating://todo:1 saturated return
    if(storage.dee==0.0 || !::isNormal(storage.dee)){
      return 0;
    }
    double maxint=double(1U<<31);//todo:0 std:: value for this.
    if(storage.dee<=-(maxint)){
      return -(maxint);
    }
    if(storage.dee>maxint){
      return maxint;
    }
    return int(storage.dee);
  }
}


NumericalValue::NumericalValue(bool bee){
  storage.dee=0;//4debug
  is=Truthy;
  storage.bee=bee;
}

NumericalValue::NumericalValue(unsigned ewe){
  storage.dee=0;//4debug
  is=Counting;
  storage.ewe=ewe;
}

NumericalValue::NumericalValue(int eye){
  storage.dee=0;//4debug
  is=Whole;
  storage.eye=eye;
}

NumericalValue::NumericalValue(double d){
 is=Floating;
 storage.dee=d;
}

bool NumericalValue::changeInto(NumericalValue::Detail newis){
  if(newis!=is){//expedite frequent case
    NumericalValue was(*this);//copy
    is=newis;
    switch (is) {
    case Truthy:
      storage.bee=was.cast<bool>();
      break;
    case Whole:
      storage.eye=was.cast<int>();
      break;
    case Counting:
      storage.ewe=was.cast<unsigned>();
      break;
    case Floating:
      storage.dee=was.cast<double>();
      break;
    }
    return true;
  } else {
    return false;
  }
}

double NumericalValue::value() const noexcept {
  return this->cast<double>();
}

bool NumericalValue::setto(const NumericalValue &other){
  NumericalValue formerly(*this);
  *this=other;
  return !(*this==formerly);
}

bool NumericalValue::operator ==(const NumericalValue &other) const noexcept{
  switch (is) {
  case Truthy:
    return storage.bee==other.cast<bool>();
  case Whole:
    return storage.eye==other.cast<int>();
  case Counting:
    return storage.ewe==other.cast<unsigned>();
  case Floating:
    return storage.dee==other.cast<double>();//# exact compare intended,might use 'nearly' if group agrees to it.
  }
  return false;
}

bool NumericalValue::operator >(const NumericalValue &other) const noexcept {
  switch (is) {
  case Truthy:
    return storage.bee>other.cast<bool>();
  case Whole:
    return storage.eye>other.cast<int>();
  case Counting:
    return storage.ewe>other.cast<unsigned>();
  case Floating:
    return storage.dee>other.cast<double>();
  }
  return false;
}


NumericalValue &NumericalValue::operator =(const NumericalValue &other){
  switch (is) {
  case Truthy:
    storage.bee=other.cast<bool>();
    break;
  case Whole:
    storage.eye=other.cast<int>();
    break;
  case Counting:
    storage.ewe=other.cast<unsigned>();
    break;
  case Floating:
    storage.dee=other.cast<double>();
    break;
  }
  return *this;
}

template <> NumericalValue::Detail detail<double>(){ return NumericalValue::Detail::Floating;}
template <> NumericalValue::Detail detail<int>(){ return NumericalValue::Detail::Whole;}
template <> NumericalValue::Detail detail<unsigned>(){ return NumericalValue::Detail::Counting;}
template <> NumericalValue::Detail detail<unsigned short>(){ return NumericalValue::Detail::Counting;}
template <> NumericalValue::Detail detail<bool>(){ return NumericalValue::Detail::Truthy;}


#include <cinttypes>
void NumericalValue::testUsage(){
  NumericalValue u;
  u.storage.eye=42;
  int &iref=u.as<int>();
  dbg("int 42 as int: %d or %d ",u.as<int>(),iref);
  dbg("As double: %g",u.as<double>());
  double &dref=u.as<double>();
  dref=42;
  dbg("double 42 as int: %d , unt64: 0x%jx ",iref,u.as<uintmax_t>());
  dbg("As double: %g",u.as<double>());
  bool &bref=u;
  bref=true;
  dbg("True as unsigned: %u",u.as<unsigned>());
  uintmax_t &um=u;
  for(int i=0;i<10;++i){
    dref=i;
    dbg("%d: %g %jx",i,dref,um);
  }

}
