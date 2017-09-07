#include "numericalvalue.h"
#include "logger.h"
#include "eztypes.h"
#include "minimath.h" //nan
#include "cheaptricks.h" //changed

NumericalValue::NumericalValue(){
 storage.dee=0;//one way to get it to be all zeroes.
 is=Floating;//first use of class was replacing something that was float even when it didn't need to be.
}

bool NumericalValue::changeInto(NumericalValue::Detail newis){
  if(newis!=is){//expedite frequent case
    switch (newis) {
    case Truthy:
      switch (is) {
      case Truthy:
        //#nada
        break;
      case Whole:
        storage.bee=storage.eye!=0 ;
        break;
      case Counting:
        storage.bee=storage.ewe!=0 ;
        break;
      case Floating:
        storage.bee=storage.dee !=0.0;
        break;
      }

      break;
    case Whole:
      switch (is) {
      case Truthy:
        storage.eye=storage.bee;
        break;
      case Whole:
        //#nada
        break;
      case Counting:
        storage.eye=storage.ewe;
        break;
      case Floating:
        storage.eye=storage.dee;
        break;
      }

      break;
    case Counting:
      switch (is) {
      case Truthy:
        storage.ewe=storage.bee ;
        break;
      case Whole:
        storage.ewe=storage.eye ;
        break;
      case Counting:
        //#nada
        break;
      case Floating:
        storage.ewe=storage.dee ;
        break;
      }

      break;
    case Floating:
      switch (is) {
      case Truthy:
        storage.dee=storage.bee?1.0:0.0 ;
        break;
      case Whole:
        storage.dee=storage.eye ;
        break;
      case Counting:
        storage.dee=storage.ewe ;
        break;
      case Floating:
        //#nada
        break;
      }
      break;
    }
    return true;
  } else {
    return false;
  }
}

double NumericalValue::value() const noexcept {
  switch (is) {
  case Truthy:
    return storage.bee;
  case Whole:
    return storage.eye;
  case Counting:
    return storage.ewe;
  case Floating:
    return storage.dee;
  }
  return Nan;
}

bool NumericalValue::setto(double d){
  switch (is) {
  case Truthy:
    return changed(storage.bee,d!=0.0);
  case Whole:
    return changed(storage.eye,int(d));
  case Counting:
    return changed(storage.ewe,unsigned(d));
  case Floating:
    return changed(storage.dee,d);
  }
  return false;
}

void NumericalValue::operator =(double d){
  switch (is) {
  case Truthy:
    storage.bee=d!=0.0;
    break;
  case Whole:
    storage.eye=int(d);
    break;
  case Counting:
    storage.ewe=unsigned(d);
    break;
  case Floating:
    storage.dee=d;
    break;
  }

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
