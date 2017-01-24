#include "lampsettings.h"

#include "cheaptricks.h"

enum LampParam {LF,LI,LC};

LampSettings::LampSettings(void):HasSettings("FIC"){
  hertz=1100.0;
  curve[0]=0.8;
  curve[1]=0.2;
}

double LampSettings::getLinear()const{
  return i.linear!=0?i.linear: curve.y(i.natural);
}

double LampSettings::getNatural(double linear)const{
  return curve.inv(linear);
}

bool LampReport::setParams(ArgSet &args){
  set(natural,args);
  set(linear,args,0.0);
  return isModified();
}

void LampReport::getParams(ArgSet &args)const{
  args.next() = natural;
  if(linear!=0){//hide diagnostic doobers whenever easy
    args.next() = linear;
  }
}

bool LampSettings::setParam(ID fieldID, ArgSet& args){
  switch(pMap(fieldID)) {
  default: return false;
  case LF:
    set(hertz,args,1100.0);// 1100 is slowest round number that works at both 64 and 72 MHz.
    //#join
  case LC:
    also(curve.setParams(args));
    break;
  case LI:
    also(i.setParams(args));
    break;
    //    //other params are for the light curve.
  }
  return true;
} /* setParam */

bool LampSettings::getParam(ID fieldID, ArgSet& args)const{
  switch(pMap(fieldID)) {
  default: return false;
  case LF:
    args.next() = hertz;
    //#join
  case LC:
    curve.getParams(args);
    break;
  case LI:
    i.getParams(args);
    break;
  }
  return true;
} /* getParam */

bool LampSettings::wasModified(){
  //need to clear subordinates as they are not separately investigated
  return curve.wasModified()&&ChangeMonitored::wasModified();
}

#include "minimath.h"
bool LampReport::aint(const LampReport &reported) {
  return !nearly(natural,reported.natural,12);//gross compare, deals with timer quantization et al.
}
