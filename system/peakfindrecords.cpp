#include "peakfindrecords.h"


PeakSearch ::PeakSearch(Storable &node) : Stored(node),
  ConnectChild(energy),
  ConnectChild(width),
  ConnectChild(which){
  //#nada
}

bool PeakSearch ::isValid() const {
  return energy>0;
}

bool PeakSearch ::usableWidth() const {
  return width>0;
}

PeakFind::PeakFind(Storable &node) :
  Stored(node),
  ConnectChild(center),
  ConnectChild(amplitude),
  ConnectChild(low),
  ConnectChild(riser),
  ConnectChild(high),
  ConnectChild(faller),
  ConnectChild(si),
  ConnectChild(efit),
  ConnectChild(rfit){
  //#nada
}

void PeakFind::clear(){
  center = 0;
  amplitude = 0;
  low = Nan;
  riser = Nan;
  high = Nan;
  faller = Nan;
  si = 0;
  efit = false;
  rfit = false;
} // PeakFind::clear

double PeakFind::width() const {
  return high - low;
}

bool PeakFind::hasWidth() const {
  return isNormal(high.native()) && isNormal(low.native());//#we do desire to exclude zero here
}

#if 0
//need to extract PolyFilter::ScanReport
//fill a PeakFind from a PolyFilter::ScanReport
if(low.maxrmin>0) {//cheaper than a nan detect
  peak.low = report.low.absolute(offset);
  peak.riser = ratio(report.low.maxrmin,S2);
}
if(high.maxrmin<0) {//cheaper than a nan detect
  peak.high = report.high.absolute(offset);
  peak.faller = -ratio(double(report.high.maxrmin),double(S2));//#inserted the minus sign for the sake of the gui
}
if(top.maxrmin>0) {
  peak.center = report.top.absolute(offset);
  peak.amplitude = ratio(double(report.top.maxrmin),double(S0));
  report.meaningful = true;
} else {
  report.meaningful = false;
}

#endif
