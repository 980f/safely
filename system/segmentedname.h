#ifndef SEGMENTEDNAME_H
#define SEGMENTEDNAME_H

//#include "textkey.h"
#include "textpointer.h"
#include <chain.h>

/** base pathname class, @see Pathname for something useful */
class SegmentedName:public Chain<Cstr>  {

public:
  SegmentedName();

  bool empty() const;
  /** caller is responsible for not freeing stuff passed as TextKey until after this is deleted.
   *  This class will create Cstr's to hold on to addresses.
   *  Use case: strings embedded in source code don't go away and don't need to be deletes so use these methods for such static items*/
  void prefix(TextKey parent);
  void suffix(TextKey child);

  /** using these creates a copy of the string held locally and destroyed at the appropriate time */
  void prefix(const Cstr &parent);
  void suffix(const Cstr &child);

  /** removes trivial path elements */
  void purify();

  /** add to this one copies of all the entities of the other */
  void copycat(const SegmentedName &other);

  /** relocate to this one the entities of the other, the other is empty when done.*/
  void transfer(SegmentedName &other);

  /** @returns an indexer, that can alter this */
  ChainScanner<Cstr> indexer();

  ConstChainScanner<Cstr> indexer() const;
}; // class SegmentedName

#endif // SEGMENTEDNAME_H
