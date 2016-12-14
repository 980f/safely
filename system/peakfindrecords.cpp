#include "peakfindrecords.h"


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

