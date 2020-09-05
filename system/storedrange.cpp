#include "storedrange.h"

StoredRange::StoredRange(Storable &node):Stored(node),
  ConnectChild(min),
  ConnectChild(max)
{
  //#nada
}

StoredRange::StoredRange(Storable &node, double maximum, double minimum):Stored(node),
  ConnectChild(min, minimum),
  ConnectChild(max, maximum)
{
  //#nada
}

StoredRange::StoredRange( double maximum, double minimum):StoredRange(NullRef(Storable),maximum,minimum){}


StoredRange::~StoredRange(){
  //#nada, present for breakpoints.
}

void StoredRange::setto(double higher, double lower){
  max=higher;
  min=lower;
}

Ranged StoredRange::ranged() const {
  return {max,min};
}

//sigc::slot<bool> StoredRange::rangeChecker() const{
//  return MyHandler(StoredRange::isUseful);
//}

bool StoredRange::isUseful()const{
  return ranged().nonTrivial();
}
