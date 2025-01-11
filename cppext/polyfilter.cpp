#include "polyfilter.h"


PolyFilter::PolyFilter(unsigned hw):hw(hw){
  //#nada
}

PolyFilter::Inflection::Inflection():estimate(0),delta(0){

}

double PolyFilter::Inflection::absolute(int offset)const{
  return offset+point.location-ratio(double(delta),double(point.amplitude));
}

/** use return to reduce cost of computing the tweak */
bool PolyFilter::Inflection::recordif(bool changeit,const Datum &testpoint){
  if(changeit){
    point = testpoint;
    return true;
  } else {
    return false;
  }
}

/** use return to reduce cost of computing the tweak */
bool PolyFilter::Inflection::morePositive(Datum && testpoint){
  return recordif(point.amplitude<testpoint.amplitude,testpoint);
}

bool PolyFilter::Inflection::moreNegative(Datum && testpoint){
  return recordif(point.amplitude>testpoint.amplitude,testpoint);
}
