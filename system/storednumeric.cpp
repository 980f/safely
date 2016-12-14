#include "storednumeric.h"

/////////////////
void adjustIndex(int removed,StoredInt &index){
  if(index.native()>removed){
    --index;
  } else if(index.native()==removed){
    index=-1;
  }
}
/** Converts a c string to a double.  @returns true if the entire string was converted, @returns false if the string had pieces that were not converted. */


/////////////////

//////////////

sigc::connection whenCleared(StoredBoolean &thing, SimpleSlot action) {
  return thing.onAnyChange(sigc::bind(&onEdge, thing.getLater(), false, action));
}

sigc::connection whenSet(StoredBoolean &thing, SimpleSlot action) {
  return thing.onAnyChange(sigc::bind(&onEdge, thing.getLater(), true, action));
}
