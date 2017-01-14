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
  }
  void getParams(ArgSet&args)const{
    args.next()=two;
    args.next()=one;
  }
};



class SettingsTestData {
  STDOne roger;
  STDTwo wilco;
};


SettingTag<SettingsTestData> myMapper []={
  {'R',SettingsTestData::roger},
  {'W',SettingsTestData::wilco},
};

#endif // SETTINGSTESTDATUM_H
