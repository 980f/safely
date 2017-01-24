#ifndef PCUCONTROL_H
#define PCUCONTROL_H
#include "bidisetting.h"  //BidirectionalThingy , core of door and purge
#include "positionersettings.h"
#include "purgerconfig.h"
#include "doorlatchsetting.h"
#include "lampsettings.h"
#include "artcore.h"

const MnemonicSet PcuGroups="MFWTRDLPSO" ;
enum PcuUnit {//#must match PcuGroups
  M, F, W, T, R, D, L, P, S,O
};

const MnemonicSet PcuParams="DPLFWTRS";
enum PcuParam {//in addition to report lookup these guys index report pollers:
  MD, MP, ML, MF, MW, MT, MR, MS,
};


class PcuReporter : public HasSettings {
public:
  PositionerReport wheelReport;
  MotorReport wheelMotorReport;
  PositionerReport strayReport;
  MotorReport strayMotorReport;
  DoorlatchReport doorReport;
  LampReport lampReport; //actual duty cycle, perhaps should be nominal then actual.
  ScalarReport<bool> isSpinning;//readback of control
  PurgerReport purgeReport;

public:
  PcuReporter ();
  virtual bool setParam(ID fieldID, ArgSet&args);
  virtual bool getParam(ID fieldID, ArgSet&args) const;

};

struct PcuControl:public SettingsGrouper  {
public: //made public so that qcumaster can undo a 'markAll' executed at r's parent.
  PcuReporter *r;//no report when in host api.
public:
  virtual HasSettings *unit4(ID asciiId,bool forRead);
  ArtController poller;
  PositionerSettings wheel;
  PositionerSettings stray;
  DoorlatchSetting door;
  LampSettings lamp;
  ScalarSetting<bool> beSpinning;
  PurgerConfig purge;
  PcuControl();
  void linkReports(PcuReporter *r){
    this->r=r;
  }

  void defaultSpew();

/** @returns the number of changes needed */
  void propagate(PcuControl &desired);
  /** @param report is not used. */
  void propagate(PositionerReport &report,PositionerSettings &self,PositionerSettings &master);
};


class PcuMaster: public PcuControl {
protected:
  PcuControl *m;
public:
  PcuMaster();
  void linkMaster(PcuControl &m){
    this->m=&m;
  }

  HasSettings *unit4(ID asciiId,bool forRead); //access to reportschildren

  void propagate();
};


#endif // PCUCONTROL_H
