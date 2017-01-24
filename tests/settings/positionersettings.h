#ifndef POSITIONERSETTINGS_H
#define POSITIONERSETTINGS_H

#include "eztypes.h"
#include "hassettings.h"
#include "steppercontrol.h"
#define MaxIndexerPositions 10
/** control a StepperMotor to put a load in one of a small set of preferred places.
  * two variants needed right away: a single home and a home per place.
  */

struct Position {
  int center;
  int markWidth; //4 debug
};

class TrayGeometry: public Settable{
public:
  unsigned numIndexes; //also is dynamic maximum allowed index.
  u32 stepsPerCycle; //steps per revolution of motor * gear ratio
  u16 widthOf1; //width of notch at slot 1
  u16 widthPer; //additional notch width at other slots, 0 if there is only one slot.
  bool isViable(bool fully=true)const;
  //FYI: home offset is part of the index itself, not the user of the indexing.
  /** @return whether given index has an alignment notch*/
  bool hasMark(unsigned index)const;
  int canonicalStep(int step)const;
//  unsigned canonicalMark(unsigned index)const;
  /** canonical position, lower levels may translate this to +/- a cycle for optimal moves.*/
  int stepFor(unsigned index)const;
  /***/
  unsigned closestIndex(int step)const;
  bool atIndex(unsigned index, int step)const;
  /** @return nominal tray position as function of notch width.
    */
//  unsigned indexFor(int width)const;
  bool validIndex(unsigned index)const;
  TrayGeometry(void);

  int numParams()const;
  bool setParams(ArgSet &args);
  void getParams(ArgSet &args)const;
};
enum PositionerParam {IG,II, };

class PositionerSettings : public HasSettings {
public:
  StepperControl c;
  int desiredIndex;
  TrayGeometry g;
  bool setParam(ID fieldID, ArgSet&args);
  void touch(ID fieldID);
  bool getParam(ID fieldID, ArgSet&args)const;
  PositionerSettings():HasSettings("GI"){
    desiredIndex=~0;//~0: do nothing until commands have been processed
  }

  int desiredStep(){
    return g.stepFor(desiredIndex);
  }
  void setDesiredIndex(int which);
  bool hasMark() ISRISH {
    return desiredIndex>=0 && g.hasMark(desiredIndex);
  }
  bool simpleHome() ISRISH {
    return g.widthPer==0;
  }
  /**set desired index to preferred idling location.*/
  void goHome(){
    setDesiredIndex(g.hasMark(g.numIndexes)?g.numIndexes:1);
  }
};

struct PositionerReport :public Settable{
  int motionState;//inMotion?1 hasHomed?2:
  int lastPosition;//"last position that had a mark", purely diagnostic
  int targetPosition;

  PositionerReport():
    motionState(),
    lastPosition(),
    targetPosition() {
  }

  int numParams()const {
    return 3;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;

  bool needsUpdate(PositionerSettings &host) const{
    if(host.desiredIndex==~0){//low level diagnostic is running, don't touch things.
      return false;
    }
    //IF motor is in good shape and not moving can trust its targetPosition
    return (motionState==2 && targetPosition!=host.desiredIndex);
  }

  bool isAt(int desired){
    return motionState==2 && targetPosition==desired;
  }

};

#endif // POSITIONERSETTINGS_H
