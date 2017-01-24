#include "bidisetting.h"

BidiSetting::BidiSetting(const char*extmap):
  HasSettings(extmap),
  period() {
  //#nada
}
bool BidiSetting::setParam(ID fieldID, ArgSet& args){
  switch(pMap(fieldID)) {
  case BDT :
    period=args.next();
    return true;
  }
  return false;
}

bool BidiSetting::getParam(ID fieldID, ArgSet& args)const{
  switch(pMap(fieldID)) {
  case BDT:
    args.next() = period;
    return true;
  }
  return false;
}
