#include "doorlatchsetting.h"

DoorlatchSetting::DoorlatchSetting():
  BidiSetting(BidiParameterString "ZI"),
  be(Off),
  isClosed() {
  this->period=2;//something minimal but enough to keep motor from burning out before the real config gets sent.
}

bool DoorlatchSetting::setParam(ID  fieldID, ArgSet&args){
  switch(pMap(fieldID)){
  case DZ:
    set(be, Autoish(args.next(Off)));
    //#join
  case DI:
    set(isClosed,args);
    break;
  default:
    return BidiSetting::setParam(fieldID,args);
  }
  return true;
}

bool DoorlatchSetting::getParam(ID fieldID, ArgSet&args) const{
  switch(pMap(fieldID)){
  case DZ:
    args.next()=be;
    //#join
  case DI:
    args.next()=isClosed;
    break;
  default:
    return BidiSetting::getParam(fieldID,args);
  }
  return true;
}
///////////////
bool DoorlatchReport ::setParams(ArgSet &args){
  set(actual,Autoish(args.next()));
  set(timer,args);
  if(timer>10){//#occasionally the timer reports a mega number, at least as of firmware 1534.
    timer=0;
  }
  return isModified();
}

void DoorlatchReport ::getParams(ArgSet &args)const{
  args.next()=actual;
  args.next()=timer;
}
