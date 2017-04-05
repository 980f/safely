#include "storednumeric.h"

/////////////////
void adjustIndex(int removed,StoredInt &index){
  if(index.native()>removed){
    --index;
  } else if(index.native()==removed){
    index=-1;
  }
}

/////////////////

sigc::connection whenCleared(StoredBoolean &thing, SimpleSlot action) {
  return thing.onAnyChange(sigc::bind(&onEdge, thing.getLater(), false, action));
}

sigc::connection whenSet(StoredBoolean &thing, SimpleSlot action) {
  return thing.onAnyChange(sigc::bind(&onEdge, thing.getLater(), true, action));
}

#if cppGetsFixedOnDerivingFromTemplate //presently have to republish the whole interface

StoredBoolean::StoredBoolean(Storable &node, bool fallback):StoredNumeric<bool>(node,fallback){}

sigc::connection StoredBoolean::whenSet(SimpleSlot action,bool kickme){
  return onAnyChange(sigc::bind(&onEdge, getLater(), true, action),kickme);
}

sigc::connection StoredBoolean::whenCleared(SimpleSlot action, bool kickme){
  return onAnyChange(sigc::bind(&onEdge, getLater(), false, action),kickme);
}
#endif
