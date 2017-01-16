#ifndef SEGMENTEDNAME_H
#define SEGMENTEDNAME_H

//#include "textkey.h"
#include "textpointer.h"
#include <chain.h>

/** base pathname class, @see Pathname for something useful */
class SegmentedName:public Chain<Cstr>  {
  /** there are two reasonable approaches to assignment so we force you to use a named function */
  SegmentedName(const SegmentedName &)=delete;
  void operator =(const SegmentedName &)=delete;

public://this is a utility class, adding better named functions to its base.
  SegmentedName();
  /** @returns whether there are any path pieces to this name */
  bool empty() const;
  /** caller is responsible for not freeing stuff passed as TextKey until after this is deleted.
   *  This class will create Cstr's to hold on to addresses.
   *  Use case: strings embedded in source code don't go away and don't need to be deletes so use these methods for such static items*/
  void prefix(TextKey parent);
  void suffix(TextKey child);

  /** using these creates a copy of the string held locally and destroyed at the appropriate time */
  void prefix(const Cstr &parent);
  void suffix(const Cstr &child);

  /** @deprecated untested
   * removes trivial path elements */
  void purify();

  /** @deprecated untested
   * add to this one copies of all the entities of the other */
  void copycat(const SegmentedName &other);

  /** @deprecated untested
   * relocate to this one the entities of the other, the other is empty when done.*/
  void transfer(SegmentedName &other);

  /** @returns an indexer, that can alter this */
  ChainScanner<Cstr> indexer();

  /** @returns an indexer, that can only view this, note that the individual pieces are still mutaable. */
  ConstChainScanner<Cstr> indexer() const;
}; // class SegmentedName

#endif // SEGMENTEDNAME_H
