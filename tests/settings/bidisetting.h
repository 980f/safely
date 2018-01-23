#ifndef BIDISETTING_H
#define BIDISETTING_H

#include "hassettings.h"
#include "settable.h"

#define BidiParameterString "T"
enum BidiParam {BDT};

class BidiSetting : public HasSettings {
public:
  BidiSetting (const char*extmap=BidiParameterString);
  Setting<double> period;
  /** params: T for power down delay*/
  virtual bool setParam(ID terse, ArgSet&args);
  virtual bool getParam(ID terse, ArgSet&args) const;
};

#endif // BIDISETTING_H
