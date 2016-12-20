#ifndef SEGMENTEDNAME_H
#define SEGMENTEDNAME_H

#include "textkey.h"
#include "cstr.h"
#include <chain.h>

/** bsae pathname class*/
class SegmentedName: public Chain<Cstr> {
public:
  SegmentedName();
  void prefix(TextKey *parent);
  void suffix(TextKey *child);
  /** number of 'slashes' needed to assemble into a single string*/
  void numSeperators(bool rooter=false)const;
  /** sum of lengths of elements, if @param raw just count as is, if not then compute \u escape expansions */
  void contentLength(bool slashu=false,bool urlesc=false)const;
  /** total length. seperator length might be 2 if seperating with crlf's or a unicode or url escape sequence */
  void mallocLength(unsigned seperatorLength=1)const;
};

#endif // SEGMENTEDNAME_H
