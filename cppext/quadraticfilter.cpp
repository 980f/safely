#include "quadraticfilter.h"

QuadraticFilter::QuadraticFilter(unsigned hw) : PolyFilter(hw),
  hw2(hw * hw),
  S0(2 * hw + 1),
  R0(hw2 + hw),
  S2((S0 * R0) / 3),
  R2(3 * R0 - 1),
  S4((S2 * R2) / 5),
  D4(S0 * S4 - squared(S2)) {
  EraseThing(Y); //4 debug
  EraseThing(delta); //4 debug
  EraseThing(est);
  //  dbg("hw,hw2,S0,R0,S2,R2,S4,D4");
  //  dbg("%d,%d,%g,%d,%g,%d,%g,%g",hw,hw2,S0,R0,S2,R2,S4,D4);
}

double QuadraticFilter::curvature() const {
  return ratio((S0 * Y[2] - S2 * Y[0]), D4);
}

int QuadraticFilter::curvish() const {
  return 3 * Y[2] - R0 * Y[0]; //this formula comes from moving the denominator from the second term, to avoid doing divides at runtime even though it is guaranteed to not truncate.
}

double QuadraticFilter::slope() const { //same as linear
  return slope(Y[1]);
}

double QuadraticFilter::slope(int extremum) const {
  return ratio(double(extremum), S2);
}

int QuadraticFilter::signA1() const { //same as linear
  return signof(Y[1]);
}

double QuadraticFilter::amplitude() const {
  return amplitude(Y[0]);
}

double QuadraticFilter::amplitude(int datum) const {
  return ratio(S4 * datum - S2 * Y[2], D4);
}

int QuadraticFilter::ampEstimate() const {
  return R2 * Y[0] - 5 * Y[2];//this formula comes from moving the denominator from the first term, to avoid doing divides at runtime even though it is guaranteed to not truncate.
}

void QuadraticFilter::recordInflection(Inflection &flect) const {
  flect.estimate = est[2];
  flect.delta = 3 * delta[2] - R0 * delta[0];
}

void QuadraticFilter::init(const CenteredSlice &slice,bool fastly) {
  //accumulators
  Y[2] = 0;
  Y[1] = 0;
  Y[0] = slice[0];
  for (int fi = 1; fi <= hw; ++fi) {
    int high = slice[fi];
    int low = slice[-fi];
    int sum = (high + low);
    int diff = (high - low);
    Y[0] += sum;
    Y[1] += fi * diff; //note: -fi*low = fi*(-low)
    Y[2] += fi * fi * sum;
  }
} // QuadraticFilter::init

void QuadraticFilter::step(CenteredSlice &slice) {
if (fastly) {
  //optimal
  int low = slice.lowest();//amplitude of point leaving view
  slice.step(true);
  int high = slice.highest();//amplitude of point entering view
  //common sub-expressions
  int subavg = (Y[0] - low);
  int sum = (high + low);
  int diff = (high - low);

  //order of lines of code is important below, in order to not double buffer the Y[] values.
  Y[2] += delta[2] = hw2 * diff - 2 * (Y[1] + hw * low) + subavg;
  Y[1] += delta[1] = hw * sum - subavg;//same as linear
  Y[0] += delta[0] = diff;//same as linear
} else {
  //non optimal
  copyObject(Y, delta, sizeof(delta));
  slice.step(true);
  init(slice);
  delta[2] = Y[2] - delta[2];
  delta[1] = Y[1] - delta[1];
  delta[0] = Y[0] - delta[0];
}
  est[0] = ampEstimate();
  est[1] = Y[1]; //Y1 is the slope estimate.
  est[2] = curvish();
} // QuadraticFilter::step

/** @param slice is search window, presumed to have a filter's worth of channels outside on each side,
 *   @param peak records the most interesting points in the range
 *   @param offset is the absolute index of the center of the slice, added to each slice-relative coordinate found */
void QuadraticFilter::scan(const CenteredSlice &slice, ScanReport &report) {
  CenteredSlice slider = slice.Endpoint(0, hw);

  init(slider);
  //  dbg("location,Y[0],Y[1],Y[2],delta[0],delta[1],delta[2],est[0],est[1],est[2],actual[0],actual[1],actual[2]");
  for (int location = -slice.hwidth; location++ < slice.hwidth;) { //#can post inc as the init call handles the first point.
    bool y2waspos = est[2] > 0;
    bool y1waspos = Y[1] > 0;
    step(slider);
    //    dbg("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%g,%g,%g",location,
    // Y[0],Y[1],Y[2],delta[0],delta[1],delta[2],est[0],est[1],est[2],amplitude(),slope(),curvature());
    if (y2waspos) {
      if (est[2] < 0) {
        if (report.low.morePositive({est[1], location})) {
          recordInflection(report.low);
        }
      }
    } else {
      if (est[2] > 0) {
        if (report.high.moreNegative({est[1], location})) {
          recordInflection(report.high);
        }
      }
    }

    if (y1waspos && est[1] < 0) { //slope at peak
      if (report.top.morePositive({est[0], location})) {
        report.top.delta = delta[1];
        report.top.estimate = est[1];
      }
    }
  }
} // QuadraticFilter::scan
