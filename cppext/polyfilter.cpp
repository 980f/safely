#include "polyfilter.h"

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

