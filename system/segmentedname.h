#ifndef SEGMENTEDNAME_H
#define SEGMENTEDNAME_H

#include "textkey.h"
#include "cstr.h"
#include <chain.h>

/** base pathname class, storage manager. @see Pathname for something useful */
class SegmentedName: public Chain<Cstr> {
    /** whether the path is prefixed with the seperator char when merged */
    bool rooted;
public:
  SegmentedName(bool rooted=true);
  /** caller is responsible for not freeing stuff passed as TextKey until after this is deleted.
 This class will create Cstr's to hold on to addresses.
Use case: strings embedded in source code don't go away and don't need to be deletes so use these methods for such static items*/
  void prefix(TextKey parent);
  void suffix(TextKey child);
  /** Cstr reference items will be represented herein by Text objects, which means this class will tend to deletions and there is no risk of use after free.
   * use these methods for dynamic items that might go away while this object still lives.
   *
   * Note: when unsure of where your char * came from (static or stack/heap) do this:
   *  segName.suffix(Cstr(name)); //note NO NEW.
   *  A local copy will be made.
*/
  void prefix(const Cstr &parent);
  void suffix(const Cstr &child);

  /** number of 'slashes' needed to assemble into a single string*/
  unsigned numSeperators()const;
  /** sum of lengths of elements, if @param raw just count as is, if not then compute \u escape expansions */
  unsigned contentLength(bool slashu=false,bool urlesc=false)const;
  /** total length. seperator length might be 2 if seperating with crlf's or a unicode or url percent sequence */
  unsigned mallocLength(unsigned seperatorLength=1)const;

  /** @returns an indexer, that can alter this */
  ChainScanner<Cstr> indexer();
};

#endif // SEGMENTEDNAME_H
