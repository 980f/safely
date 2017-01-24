#ifndef LAMPSETTINGS_H
#define LAMPSETTINGS_H
#include "settable.h"
#include "hassettings.h"

/**
  * LED lamp, pwm intensity control on a human scale
  */

#include "polynomial.h" //simplified transfer function.

struct LampReport:public Settable {
  double natural;
  double linear; //for diagnostics, overrides natural if not zero.

  LampReport():
    natural(),
    linear() {
  }

  int numParams()const{
    return 2;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
  bool aint(const LampReport &reported);
};

class LampSettings : public HasSettings {
  double hertz; //LF
  Polynomial<2> curve;
public:
  LampSettings(void);
  double getLinear(void)const;
  double getNatural(double linear)const;
  bool setParam(ID terse, ArgSet&args);
  bool getParam(ID terse, ArgSet&args) const;
  bool wasModified();//overloaded to clear subordinates
  LampReport i;
  /**@return whether 'this' should be sent out to real one.*/
};



//no status object required, it always does what it is told.
#endif // LAMPSETTINGS_H
