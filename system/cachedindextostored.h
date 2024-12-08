#ifndef CACHEDINDEXTOSTORED_H
#define CACHEDINDEXTOSTORED_H

#include "storedgroup.h"
/** @deprecated  no longer built, keeping around for reference.
 *  this class was obsoleted by bringing back use of the Stored::parentIndex function. That in turn was done to simplify sorting of stored nodes for performance on load.
 *  Sorting would have required fancy manipulation in this class.
 */
class CachedIndexToStored {
  unsigned index;
  void fixIndex(unsigned which);

public:
  CachedIndexToStored();
  /** call on creation de novo*/
  void init(unsigned value,StoredGroup<Stored> &group);
  /** call on load from file */
  void link(StoredGroup<Stored> &group){
    group.onRemoval(MyHandler(CachedIndexToStored::fixIndex));
  }

  operator unsigned() const {
    return index;
  }
}; // class CachedIndexToStored

#endif // CACHEDINDEXTOSTORED_H
