#ifndef SETTINGSTESTDATUM_H
#define SETTINGSTESTDATUM_H

#include "settablegroup.h"



class STDOne: public Settable {
  double one;
public:
  STDOne();
  int numParams()const{ return 1;}
  bool setParams(ArgSet&args){
    one=args.next();
    return isModified();
  }
  void getParams(ArgSet&args)const{
    args.next()=one;
  }
};


class STDTwo: public Settable {
  double two;
  double one;
public:
  STDTwo();
  int numParams()const{ return 1;}
  bool setParams(ArgSet&args){
    two=args.next();
    one=args.next();
    return isModified();
  }
  void getParams(ArgSet&args)const{
    args.next()=two;
    args.next()=one;
  }
};



class SettingsTestData {
public:
  STDOne roger;
  STDTwo wilco;
};


SettingTag<SettingsTestData> myMapper []={
  {'R', reinterpret_cast<Settable SettingsTestData::*>( &SettingsTestData::roger),nullptr},
  {'W', reinterpret_cast<Settable SettingsTestData::*>( &SettingsTestData::wilco),nullptr},
};

#endif // SETTINGSTESTDATUM_H
