#include "linearfilter.h"
////////////////////

LinearFilter::  LinearFilter(unsigned hw) :
  PolyFilter(hw),
  S0(2 * hw + 1),
  S2((S0 * hw * (hw + 1)) / 3){
  EraseThing(Y);
}

double LinearFilter::slope() const {
  return ratio(double(Y[1]),S2);
}

int LinearFilter::signA1() const {
  return signof(Y[1]);
}

double LinearFilter::amplitude() const {
  return ratio(double(Y[0]),S0);
}

void LinearFilter::init(const CenteredSlice &slice){
  Y[1] = 0.0; //accumulators
  Y[0] = slice[0];
  for(int fi = 1; fi<=hw; ++fi) {
    int high = slice[fi];
    int low = slice[-fi];
    Y[0] += high + low;
    Y[1] += fi * (high - low);
  }
}

void LinearFilter::step(CenteredSlice &slice){
  int low = slice.lowest();
  slice.step(true);
  int high = slice.highest();
  Y[1] += delta[1] = hw * (high + low) - (Y[0] - low);
  Y[0] += delta[0] = (high - low);
}

/** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
 *     @param peak records the most interesting points in the range
 *     @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
void LinearFilter::scan(const CenteredSlice &slice, ScanReport &report){
  CenteredSlice slider = slice.Endpoint(0,hw);

  init(slider);

  for(int location = -slice.hwidth; location++<slice.hwidth; ) {//#can post inc as the init call handles the first point.
    int prevy = Y[1];
    step(slider);
    int y1 = Y[1];//FUE

    if(report.low.morePositive({y1,location})) {
      report.low.delta = 0;//NYI, needs additional history, such as the delta not yet computed.
    }

    if(report.high.moreNegative({y1,location})) {
      report.high.delta = 0;//like low, in both cases we need to at least put something other than a Nan in the value
    }

    if(y1<0 && prevy>0) {
      if(report.top.morePositive({Y[0],location})) {
        report.top.delta = delta[1];
        report.top.estimate = Y[1];
      }
    }
  }

//  if(report.low.maxrmin>0){//cheaper than a nan detect
//    report.low=report.low.absolute(offset);
//    report.low.riser=ratio(report.low.maxrmin,S2);
//  }
//  if(report.high.maxrmin<0){//cheaper than a nan detect
//    report.high=report.high.absolute(offset);
//    report.faller=-ratio(double(report.high.maxrmin),double(S2));//#inserted the minus sign for the sake of the gui
//  }
//  if(report.top.maxrmin>0){
//    report.center=report.top.absolute(offset);
//    report.amplitude=ratio(double(report.top.maxrmin),double(S0));
//    return true;
//  } else {
//    return false;
//  }
} // LinearFilter::scan
