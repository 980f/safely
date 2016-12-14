#include "linearfilter.h"

PeakSearch ::PeakSearch(Storable &node):Stored(node),
  ConnectChild(energy),
  ConnectChild(width),
  ConnectChild(which){
  //#nada
}

bool PeakSearch ::isValid()const{
  return energy>0;
}

bool PeakSearch ::usableWidth()const {
  return width>0;
}

PeakFind::PeakFind (Storable &node):
  Stored(node),
  ConnectChild(center),
  ConnectChild(amplitude),
  ConnectChild(low),
  ConnectChild(riser),
  ConnectChild(high),
  ConnectChild(faller),
  ConnectChild(si),
  ConnectChild(efit),
  ConnectChild(rfit)
{
  //#nada
}

void PeakFind::clear(){
  center=0;
  amplitude=0;
  low=Nan;
  riser=Nan;
  high=Nan;
  faller=Nan;
  si=0;
  efit=false;
  rfit=false;
}

double PeakFind::width()const{
  return high-low;
}

bool PeakFind::hasWidth()const {
  return isNormal(high.native()) && isNormal(low.native());//#we do desire to exclude zero here
}

PolyFilter::PolyFilter(unsigned hw):hw(hw){
  //#nada
}

PolyFilter::Inflection::Inflection(){
  location=0;
  maxrmin=0;
  delta=0;
}

double PolyFilter::Inflection::absolute(int offset){
  return offset+location-ratio(double(delta),double(maxrmin));
}

/** use return to reduce cost of computing the tweak */
bool PolyFilter::Inflection::recordif(bool changeit,int newvalue,int newlocation){
  if(changeit){
    maxrmin=newvalue;
    location=newlocation;
    return true;
  } else {
    return false;
  }
}

/** use return to reduce cost of computing the tweak */
bool PolyFilter::Inflection::morePositive(int newvalue,int newlocation){
  return recordif(maxrmin<newvalue,newvalue,newlocation);
}

bool PolyFilter::Inflection::moreNegative(int newvalue,int newlocation){
  return recordif(maxrmin>newvalue,newvalue,newlocation);
}

////////////////////

LinearFilter::  LinearFilter(unsigned hw):
  PolyFilter(hw),
  S0(2*hw+1),
  S2((S0*hw*(hw+1))/3)
{
  EraseThing(Y);
}

double LinearFilter::slope()const {
  return ratio(double(Y[1]),S2);
}

int LinearFilter::signA1()const {
  return signum(Y[1]);
}

double LinearFilter::amplitude()const{
  return ratio(double(Y[0]),S0);
}


void LinearFilter::init(const CenteredSlice &slice){
  Y[1]=0.0; //accumulators
  Y[0]=slice[0];
  for(int fi=1;fi<=hw;++fi){
    int high=slice[fi];
    int low=slice[-fi];
    Y[0]+=high+low;
    Y[1]+=fi*(high-low);
  }
}

void LinearFilter::step(CenteredSlice &slice){
  int low=slice.lowest();
  slice.step(true);
  int high=slice.highest();
  Y[1]+= delta[1]=hw*(high+low)-(Y[0]-low);
  Y[0]+= delta[0]=(high-low);
}
/** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
      @param peak records the most interesting points in the range
      @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
bool LinearFilter::scan(const CenteredSlice &slice,PeakFind &peak,int offset){
  CenteredSlice slider=slice.Endpoint(0,hw);
  Inflection low;
  Inflection high;
  Inflection top;
  init(slider);

  for(int location=-slice.hwidth;location++<slice.hwidth;){//#can post inc as the init call handles the first point.
    int prevy=Y[1];
    step(slider);
    int y1=Y[1];//FUE

    if(low.morePositive(y1,location)){
      low.delta=0;//NYI, needs additional history, such as the delta not yet computed.
    }

    if(high.moreNegative(y1,location)){
      high.delta=0;//like low, in both cases we need to at least put something other than a Nan in the value
    }

    if(y1<0 && prevy>0){
      if(top.morePositive(Y[0],location)){
        top.delta=delta[1];
        top.estimate=Y[1];
      }
    }
  }

  if(low.maxrmin>0){//cheaper than a nan detect
    peak.low=low.absolute(offset);
    peak.riser=ratio(low.maxrmin,S2);
  }
  if(high.maxrmin<0){//cheaper than a nan detect
    peak.high=high.absolute(offset);
    peak.faller=-ratio(double(high.maxrmin),double(S2));//#inserted the minus sign for the sake of the gui
  }
  if(top.maxrmin>0){
    peak.center=top.absolute(offset);
    peak.amplitude=ratio(double(top.maxrmin),double(S0));
    return true;
  } else {
    return false;
  }
}
