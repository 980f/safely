#include "purgerconfig.h"
#include <cmath>

//array indexes for direct use:
enum ds {venter=0,filler=1 };

enum PurgeParameter {
  PZ=BDT+1,PG,PV,PF,
};

bool Diffuser::setParams(ArgSet&args){
  set(rc,args);
  set(threshold,args);
  return isModified();
}

void Diffuser::getParams(ArgSet&args)const{
  args.next() = rc;
  args.next() = threshold;
}


double Diffuser::exp(double seconds)const{
  return ::exp(-ratio(seconds,rc));
}

/**
  if rising then 1-exp(- deltat/rate) = amp
  if falling then exp(- deltat/rate) = amp
*/
double Diffuser::deltat(bool riser, double amp)const{
  return -log(riser?(1-amp):amp)*rc;
}

double Diffuser::fulltime(bool riser) const{
  return deltat(riser,threshold);
}


PurgerConfig::PurgerConfig():
  BidiSetting(BidiParameterString "ZGVF"),
  beOn() {
  period = 0.150; //#bistable relay's pulse width.
  dif[filler].rc = 5;
  dif[filler].threshold = 0.95;
  dif[venter].rc = 20;
  dif[venter].threshold = .06;//jch wants it to be really gone.
  flow.yaxis.setto(1712.84,0); //He ccm
  flow.xaxis.setto(3000,597.56508);//for 3.0 V swing
}

bool PurgerConfig::setParam(ID fieldID, ArgSet&args){
  switch(pMap(fieldID)) {
  default:
    return BidiSetting::setParam(fieldID, args);;
  case PF:
    also(dif[filler].setParams(args));
    break;
  case PV:
    also(dif[venter].setParams(args));
    break;
  case PG:
    also(flow.setParams(args));
    break;
  case PZ:
    set(beOn,args);
    break;
  }
  return true;
} /* setParam */

bool PurgerConfig::getParam(ID fieldID, ArgSet&args)const{
  switch(pMap(fieldID)) {
  default:
    return BidiSetting::getParam(fieldID, args);
  case PF:
    dif[filler].getParams(args);
    break;
  case PV:
    dif[venter].getParams(args);
    break;
  case PG:
    flow.getParams(args);
    break;
  case PZ:
    args.next()=beOn;
    break;
  }
  return true;
} /* getParam */

//todo:2 drop the also's in setParam and add more differential emissions.
bool PurgerConfig::wasModified(){
  if(ChangeMonitored::wasModified()){
    dif[0].wasModified();
    dif[1].wasModified();
    flow.yaxis.wasModified();
    flow.xaxis.wasModified();
    return true;
  }
  return false;
}

bool PurgerReport::setParams(ArgSet &args){
  set(isOn,args);
  set(diffuseness,args);
  set(gauge,args);
  set(isLow,args);
  return isModified();
}

void PurgerReport::getParams(ArgSet &args)const{
  args.next() = isOn;
  args.next() = diffuseness;
  args.next() = gauge;
  args.next() = isLow;
}

bool PurgerConfig::isGoodFor(const PurgerReport &rep) const{
  if(beOn==rep.isOn){
    if(beOn){
      return rep.diffuseness>dif[filler].threshold;
    } else {
      return rep.diffuseness<dif[venter].threshold;
    }
  } else {
    return false;
  }
}

double PurgerConfig::timeRemaining(const PurgerReport &rep) const{
  double tnow=dif[rep.isOn].deltat(rep.isOn,rep.diffuseness);
  double then=dif[rep.isOn].fulltime(rep.isOn);
  return then-tnow;
}
