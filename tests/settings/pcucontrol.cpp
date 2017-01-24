#include "pcucontrol.h"

PcuReporter::PcuReporter():
  HasSettings(PcuParams),
  wheelReport(),
  wheelMotorReport(),
  strayReport(),
  strayMotorReport(),
  doorReport(),
  lampReport(),
  isSpinning(),
  purgeReport() {
}

PcuControl::PcuControl():
  SettingsGrouper(PcuGroups),
  r(0),
  poller(PcuParams),
  wheel(),
  stray(),
  door(),
  lamp(),
  beSpinning(),
  purge() {
  defaultSpew();
}

HasSettings *PcuControl::unit4(ID asciiId,bool /*ignored*/){
  switch(asciiId) {
  default: return nullptr;
  case 'O': return &poller;
  case 'M': return r; //possibly null
  case 'F': return &wheel.c;
  case 'W': return &wheel;
  case 'T': return &stray.c;
  case 'R': return &stray;
  case 'D': return &door;
  case 'L': return &lamp;
  case 'P': return &purge;
  case 'S': return &beSpinning;
  } /* switch */
} /* unit4 */


//M parameters
bool PcuReporter::setParam(ID fieldID, ArgSet&args){ //M commands
  PcuParam mp=  PcuParam( pMap(fieldID));
  switch(mp) {
  default:
    return false;
  case MS:/* the report */
    isSpinning.setParams(args);
    break;
  case MF:
    wheelMotorReport.setParams(args);
    break;
  case MT:
    strayMotorReport.setParams(args);
    break;
  case MW:
    wheelReport.setParams(args);
    break;
  case MR:
    strayReport.setParams(args);
    break;
  case MD:
    doorReport.setParams(args);
    break;
  case MP:
    purgeReport.setParams(args);
    break;
  case ML:
    lampReport.setParams(args);
    break;
  }
  return true;
}

bool PcuReporter::getParam(ID fieldID, ArgSet&args)const{ //M commands
  switch(pMap(fieldID)) {
  default:
    return false;
  case MS:
    isSpinning.getParams(args);
    break;
  case MF:
    wheelMotorReport.getParams(args);
    break;
  case MT:
    strayMotorReport.getParams(args);
    break;
  case MW:
    wheelReport.getParams(args);
    break;
  case MR:
    strayReport.getParams(args);
    break;
  case MD:
    doorReport.getParams(args);
    break;
  case MP:
    purgeReport.getParams(args);
    break;
  case ML:
    lampReport.getParams(args);
    break;
  }
  return true;
} /* getParam */

static void wtf(int which){
  if(which!=0){
    which=-which;
  }
}

void PcuControl::propagate(PositionerReport &/*report*/,PositionerSettings &commander, PositionerSettings &desired){
  if(commander.c.propagate(desired.c,commander.desiredIndex==-1)){// 'c' has independent scope from what blockcheck covers.
    return;
  }

  if(commander.g.blockCheck(desired.g)) {//#doing sub block to ...
    commander.post('G');
    return;
  }

  commander.blockCheck(desired,'I');//I is all that's left if G is happy
}

/**
if host("image") setting isn't the same as remote setting then update remote with image's value
if report doesn't agree with local setting for the device then send an update.
*/
void PcuControl::propagate(PcuControl &desired){
  if(r==0){//if report not yet linked
    wtf(9981);//we should not have been called!
    return;
  }
  desired.poller.vi[0]=poller.vi[0];//#critical to getting PCU version to host.
  //local to remote, might move into heartbeat routine
  propagate(r->wheelReport,wheel,desired.wheel);
  propagate(r->strayReport,stray, desired.stray);

  //for each control block if we don't know what the other side has OR we differ then send ours:
  door.blockCheck(desired.door,'*');
  lamp.blockCheck( desired.lamp,'*');
  beSpinning.blockCheck( desired.beSpinning,'Z');
  purge.blockCheck( desired.purge,'*');
}

static const double PollRates[]={
  //MD, MP, ML, MF, MW, MT, MR, MS,
  10,  2,  1,  1,  2,  1,  2,0.5,
};

static const double KeepAlive=0.25;

/** copying due to const issues*/
void PcuControl::defaultSpew(){
  ConstArgSet rates(PollRates,sizeof(PollRates));
  poller.setFrom(KeepAlive,rates);
}

PcuMaster::PcuMaster():PcuControl(), m(0){

}

HasSettings *PcuMaster::unit4(ID asciiId, bool forRead){
  if(forRead){
    return m?m->unit4(asciiId,forRead):0;
  } else {
    return PcuControl::unit4(asciiId,forRead);
  }
}

void PcuMaster::propagate(){
  if(m){
    PcuControl::propagate(*m);
  }
  else {
    wtf(9982);
    return;
  }
}

//@preserve the following, hope to find an avenue for invoking them someday.
//bool PcuMaster::setParam(ID fieldID, ArgSet&args){ //Q command group
//  int mp=pMap(fieldID);
//  switch(mp) {
//  default:
//    return false;
//  case QM:
//    setParam('M',args);
//    break;
//  case QR:
//    setParam('H',args);
//    break;
//  } /* switch */
//  return true;
//} /* setParam */

//bool PcuMaster::getParam(ID fieldID, ArgSet&args)const{ //Q command group
//  switch(pMap(fieldID)) {
//  default:
//    return false;
//  case QM:
//    getParam('M',args);
//    break;
//  case QR:
//    getParam('H',args);
//    break;
//  } /* switch */
//  return true;
//} /* getParam */


