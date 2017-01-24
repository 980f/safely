#ifndef DOORLATCHSETTING_H
#define DOORLATCHSETTING_H

#include "bidisetting.h"
enum Autoish {Off,On,Auto};
enum DoorLatchParam { DZ=BDT+1,DI};

class DoorlatchSetting : public BidiSetting {
public:
  Autoish be;
  bool isClosed; //the sensor, from qcu to pcu, presently not used.
  DoorlatchSetting();

  bool setParam(ID fieldID, ArgSet&args);
  bool getParam(ID fieldID, ArgSet&args) const;

  void setLatch(Autoish beOn){
    set(be,beOn);
  }

  void updateIndicator(bool seemsClosed){
    set(isClosed,seemsClosed);
  }

  operator Autoish()const{
    return (be==Auto)?(isClosed?On:Off):be;
  }

};

struct DoorlatchReport :public Settable{
  Autoish actual;
  double timer;

  DoorlatchReport():
    actual(),
    timer() {
  }

  int numParams()const {
    return 3;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;

  bool disagreesWith(DoorlatchSetting &d)const {
    return actual!= d;//#actual should only be On or Off
  }
};

#endif // DOORLATCHSETTING_H
