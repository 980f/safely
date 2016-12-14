#include "cachedindextostored.h"

void CachedIndexToStored::fixIndex(int which){
  if(index>which){
    --index;
  } else if(index==which){//most cases: we are in the midst of the deletion propagation.
    index=-1;
  }
}

CachedIndexToStored::CachedIndexToStored():index(-99){
  //#nada
}

void CachedIndexToStored::init(int value, StoredGroup<Stored> &group){
  index=value;
  link(group);
}
