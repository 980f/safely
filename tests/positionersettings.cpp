#include "positionersettings.h"

bool PositionerSettings::setParam(ID fieldID, ArgSet&args){
  switch(pMap(fieldID)) {
  default:
    return false;
  case IG: //stepsPerPosition, width of index 1, delta width per position
    also(g.setParams(args));
    break;
  case II://index position, nominal step, rehome=0
    setDesiredIndex(args.next());
    break;
  } /* switch */
  return true;
} /* setParam */

void PositionerSettings::touch(ID fieldID){
  switch(pMap(fieldID)) {
  case IG:
    g.also(true);
    break;
  }
  HasSettings::touch(fieldID);
}

void PositionerSettings::setDesiredIndex(int which){
  if(g.validIndex(which)) {
    set(desiredIndex ,which);
  } else if(which==0){
    set(desiredIndex, 0);
  } else {//disable, to allow direct motor access for diagnostics.
    set(desiredIndex ,~0);//canonical bad value.
  }
}

bool PositionerSettings::getParam(ID fieldID, ArgSet&args)const{
  switch(pMap(fieldID)) {
  case IG:
    g.getParams(args);
    return true;
  case II:
    args.next() = desiredIndex;
    return true;
  } /* switch */
  return false;
} /* getParam */

///////////////////////////


int TrayGeometry:: numParams()const{
  return 4;
}
bool TrayGeometry::setParams(ArgSet &args){
  set(numIndexes , args);
  set(stepsPerCycle , args);
  set(widthOf1 ,args);
  set(widthPer ,args);
  return isModified();
}

void TrayGeometry::getParams(ArgSet &args)const{
  args.next()=numIndexes;
  args.next()=stepsPerCycle;
  args.next()=widthOf1 ;
  args.next()=widthPer ;
}


/** @return whether given index has an alignment notch*/
bool TrayGeometry::hasMark(unsigned index)const{
  return widthPer || index <= 1;
}

int TrayGeometry::canonicalStep(int step)const{
  return modulus(step, stepsPerCycle);
}

//unsigned TrayGeometry::canonicalMark(unsigned index)const{
//  return widthOf1+widthPer*(index-1);
//}

/** canonical position, lower levels may translate this to +/- a cycle for optimal moves.
called in an ISR (as well as directly) so floating point numbers must be converted to integer before we get here*/
int TrayGeometry::stepFor(unsigned index)const{ //offset is carried in homing logic, applies to absolute step
  if(index>0){
    return muldivide(stepsPerCycle, index - 1, numIndexes); //#1-based index
  } else {
    return 0;
  }
}

/***/
unsigned TrayGeometry::closestIndex(int step)const{
  return 1 + muldivide(numIndexes, canonicalStep(step), stepsPerCycle);
}

bool TrayGeometry::atIndex(unsigned index, int step)const{
  return canonicalStep(step) == stepFor(index);
}

///* @return nominal tray position as function of notch width.
//  */
//unsigned TrayGeometry::indexFor(int width)const{
//  if(widthPer){
//    int increment = width - widthOf1; //now can subtractvalue
//    if(increment < 0) {
//      return 0; //error indicator
//    }
//    increment = rate(increment, widthPer); //#1-based index
//    if(unsigned(increment) > numIndexes) {
//      return 0; //error indicator
//    }
//    return 1 + increment; //#1-based index
//  } else {
//    return 0;
//  }
//} /* indexFor

bool TrayGeometry::validIndex(unsigned index)const{
  return index > 0 && index <= numIndexes;
}

TrayGeometry::TrayGeometry(void){ //default values are for debug, there is no other basis for any of these
  numIndexes = 0;
  stepsPerCycle = 0;
  widthOf1 = 0;
  widthPer = 0;
}

bool TrayGeometry::isViable(bool fully)const{
  return stepsPerCycle>0 && (!fully||numIndexes>0);// for diagnostics.
}

///////////////////
bool PositionerReport::setParams(ArgSet &args){
  set(motionState,args);
  set(lastPosition,args);
  set(targetPosition,args);
  return isModified();
}

void PositionerReport::getParams(ArgSet &args)const{
  args.next()=motionState;
  args.next()=lastPosition;
  args.next()=targetPosition;
}
