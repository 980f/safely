#ifndef TEXTCHUNK_H
#define TEXTCHUNK_H

#include "halfopen.h"

/** minimalist block of text. is intrinisically const once created, although the referenced content is mutable */
struct TextBlock {
  friend class TextChunk;
  char &start;
  const unsigned allocated;
  /** free on delete */
  const bool amOwner;
  /** @param yours is whether to free start on destruction */
  TextBlock(char *start, unsigned allocated,bool yours=false);
  /** copying is a good idea */
  TextBlock(const TextBlock &other)=default;
  /** create on the fly in an argument list */
  TextBlock(TextBlock &&other)=default;
  /** potentially delete the buffer, if was given ownership */
  ~TextBlock();
  /** strncompare */
  int cmp(const char *other) const noexcept;
  bool operator==(const char *other) const noexcept;
  /** @returns index of next instance of @param toseek that is at or follows @param from */
  unsigned nextChar(unsigned from, char toseek) const noexcept;
  /** safe indexing function */
  char operator [](unsigned index) const noexcept {
    return index<allocated?(&start)[index]:0;
  }
protected:
  /** unsafe index function, only call when you have constrained @param index */
  char &at(unsigned index) const noexcept {
    return (&start)[index];
  }
};

/** a block of text and a sliding window over it */
class TextChunk {
public:
  TextBlock &block;
  Span span;
public:
  TextChunk(const TextBlock &block);
  /** @returns a block that is safely within its own block, the whole block is span is not valid.
 if you want an empty block on an invalid span then check before calling this */
  operator TextBlock() const noexcept ;
};

#endif // TEXTCHUNK_H
