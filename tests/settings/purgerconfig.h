#ifndef PURGERCONFIG_H
#define PURGERCONFIG_H

#include "hassettings.h"
#include "bidisetting.h"
#include "linearmapping.h"

//moved outside class due to bugs in crossworks IDE
struct Diffuser :public Settable {
  double rc;
  double threshold;//todo:2 move somewhere else.

  Diffuser():
    rc(),
    threshold() {
  }

  int numParams()const{
    return 2;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
  /** @return e^(-seconds/rc)*/
  double exp(double seconds)const;
  /** return how many seconds it would take for exp() to return the given amp value, or 1-amp.*/
  double deltat(bool riser,double amp)const;
  double fulltime(bool riser)const;
};


struct PurgerReport :public Settable {
  bool isOn;
  double diffuseness;
  double gauge;
  bool isLow;
  PurgerReport()://init members in case other end of link doesn't have them yet.
    isOn(false),
    diffuseness(0.0),
    gauge(0.0),
    isLow(false) {

  }

  int numParams()const {
    return 4;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
};

class PurgerConfig : public BidiSetting  {
public:
  enum PurgeState {Flushed,Flushing,Purging,Purged};
public:
  PurgerConfig();
  Diffuser dif[2];
  LinearMapping flow;
  /** warning/error threshold */
  double threshold;
  bool beOn; //purging else venting.
  bool setParam(ID fieldID, ArgSet&args);
  bool getParam(ID fieldID, ArgSet&args) const;
  bool wasModified();

  bool isGoodFor(const PurgerReport &rep)const;
  bool isDone()const;
  double timeRemaining(const PurgerReport &rep)const;
};

#endif // PURGERCONFIG_H
