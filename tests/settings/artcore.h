#ifndef ARTCORE_H
#define ARTCORE_H

#include "hassettings.h"
#include "twiddler.h"
#include "buffer.h"
#include "settablegroup.h"

struct VersionInfo:public Settable {
  int format;//probably will never change
  double buildId;//integer for proper releases, fraction for hacks.

  unsigned numParams()const{
    return 2;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
};

struct Reporter : public Twiddler {
  ID id;
  void doLogic(HasSettings&rq);
};

struct CoreValues:public Settable {
  bool beAutoReporting;
  bool bootFlag; //set on power up, cleared by host command.
  Twiddler fullSync;

  void init();
  void run();

  unsigned numParams()const{
    return 2;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
};

struct ReportSet:public Indexer<Reporter> ,public Settable{
  ReportSet(const char*ids);
  ReportSet(const ReportSet&other);
  bool isMaster;//because we don't want to deallocate copies!
  ~ReportSet();
  unsigned numParams()const ;
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
  bool setFrom(ConstArgSet &table);
};


template <typename Scalar> struct ScalarReport:public Settable {//#compare this to Watched<>
  Scalar value;

  operator Scalar (){
    return value;
  }

  Scalar operator =(Scalar arg){
    value=arg;
    return value;
  }

  bool operator ==(Scalar arg){
    return value==arg;
  }

  bool operator !=(Scalar arg){
    return value!=arg;
  }

  /// remainder support Settable:
  Scalar operator =(double arg){
    value=arg;
    return value;
  }

  int numParams()const {
    return 1;
  }

  bool setParams(ArgSet&args){
    set(value,args);
    return isModified();
  }

  void getParams(ArgSet&args)const{
    args.next()=value;
  }
};

template <typename Scalar> struct ScalarSetting:public HasSettings {
  Scalar value;

  operator Scalar (){
    return value;
  }

  Scalar operator =(Scalar arg){
    value=arg;
    return value;
  }

  bool operator ==(Scalar arg){
    return value==arg;
  }

  bool operator !=(Scalar arg){
    return value!=arg;
  }

  ScalarSetting():
    HasSettings("Z"),
    value() {
  }

  bool setParam(ID fieldID, ArgSet &args){
    if(fieldID=='Z'){
      set(value,args);
      return true;
    }
    return false;
  }

  bool getParam(ID fieldID, ArgSet &args) const{
    if(fieldID=='Z'){
      args.next()=value;
      return true;
    } else {
      return false;
    }
  }
};


/**
the report logic
*/
class ArtController: public HasSettings {
public:
  VersionInfo vi[2];//versions of other entities, passed through us.
  CoreValues cv;
  ReportSet reporter;
public:
  ArtController(const char*ids);
  bool setParam(ID fieldID, ArgSet &args);
  bool getParam(ID fieldID, ArgSet &args) const;
  /** static cause we can :) */
  static double heartRate(double divider);
  virtual void onHeartbeat(HasSettings &hs,SettingsGrouper &sg);//post periodic reports
  /**puts protocol version and build ID into the @param args */
  static void reportVersions(ArgSet &args);
  bool bootCheck(ArtController &twin);
  void setFrom(double keepAlive,ConstArgSet &table);//not a reference as we expect to create in-line.
};

#endif // ARTCORE_H
