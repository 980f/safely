#ifndef CACHEDINDEXTOSTORED_H
#define CACHEDINDEXTOSTORED_H

#include "storedgroup.h"
/** @deprecated  no longer built, keeping around for reference.
 *  this class was obsoleted by bringing back use of the Stored::parentIndex function. That in turn was done to simplify sorting of stored nodes for performance on load.
 *  Sorting would have required fancy manipulation in this class.
 */
class CachedIndexToStored {
  int index;
  void fixIndex(int which);

public:
  CachedIndexToStored();
  /** call on creation de novo*/
  void init(int value,StoredGroup<Stored> &group);
  /** call on load from file */
  void link(StoredGroup<Stored> &group){
    group.onRemoval(MyHandler(CachedIndexToStored::fixIndex));
  }

  operator int() const {
    return index;
  }
}; // class CachedIndexToStored

#endif // CACHEDINDEXTOSTORED_H
