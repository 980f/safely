#ifndef SEGMENTEDNAME_H
#define SEGMENTEDNAME_H

#include "textpointer.h"
#include <chain.h>

/** base pathname class, @see Pathname for something useful */
class SegmentedName:public Chain<Text> {//prior use of Cstr led to use-after-free possiblilities
  /** there are two reasonable approaches to assignment so we force you to use a named function */
  SegmentedName(const SegmentedName &)=delete;
  void operator =(const SegmentedName &)=delete;

public://this is a utility class, adding better named functions to its base.
  SegmentedName();
  /** @returns whether there are any path pieces to this name */
  bool empty() const;

  /** copies of the text will be made on the heap, this class will not delete the originals. */
  void prefix(TextKey parent);
  void suffix(TextKey child);

  /** copies of the text will be made on the heap, this class will not delete the originals. */
  void prefix(Cstr &&parent);
  void suffix(Cstr &&child);

  /** using these takes the content away from the argument and deletes it when this is deleted */
  void prefix(Text &parent);
  void suffix(Text &child);


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
  ChainScanner<Text> indexer();

  /** @returns an indexer, that can only view this, note that the individual pieces are still mutable. */
  ConstChainScanner<Text> indexer() const;
}; // class SegmentedName

#endif // SEGMENTEDNAME_H
