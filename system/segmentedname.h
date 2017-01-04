#ifndef SEGMENTEDNAME_H
#define SEGMENTEDNAME_H

//#include "textkey.h"
#include "textpointer.h"
#include <chain.h>

/** base pathname class, @see Pathname for something useful */
class SegmentedName {
public: //non-critical, can add setter and getter later if someone ever messes up.
  /** whether the path is prefixed with the seperator char when merged */
  const bool rooted;
  Chain<Cstr> elements;
public:
  SegmentedName(bool rooted = true);

  bool empty() const;
  /** caller is responsible for not freeing stuff passed as TextKey until after this is deleted.
   *  This class will create Cstr's to hold on to addresses.
   *  Use case: strings embedded in source code don't go away and don't need to be deletes so use these methods for such static items*/
  void prefix(TextKey parent);
  void suffix(TextKey child);

  /** using these creates a copy of the string held locally and destroyed at the appropriate time */
  void prefix(const Cstr &parent);
  void suffix(const Cstr &child);

  /** number of 'slashes' needed to assemble into a single string*/
  unsigned numSeperators(bool includeRootedness=false) const;
  /** sum of lengths of elements, if @param raw just count as is, if not then compute \u escape expansions */
  unsigned contentLength(bool slashu = false,bool urlesc = false) const;
  /** total length. seperator length might be 2 if seperating with crlf's or a unicode or url percent sequence */
  unsigned mallocLength(unsigned seperatorLength = 1) const;

  /** removes trivial path elements */
  void purify();

  /** @returns an indexer, that can alter this */
  ChainScanner<Cstr> indexer();

  ConstChainScanner<Cstr> indexer() const;
}; // class SegmentedName

#endif // SEGMENTEDNAME_H
