#include "halfopen.h"

#pragma once

/** limited functionality block of text with ownership management.
Most importantly its size is never altered */
class TextBlock {
  friend class TextChunk;
public:
  char &start;
  const unsigned allocated;
  const bool amOwner;

  TextBlock(char *start,unsigned allocated,bool amOwner = false);
  ~TextBlock();
  TextBlock(TextBlock &&other) noexcept ;
  TextBlock(const TextBlock &other);

  int cmp(const char *other) const noexcept;

  bool operator==(const char *other) const noexcept;

  char operator [](unsigned index) const noexcept{
    return index<allocated?(&start)[index]:0;
  }

  /** you are on your own if you use this. */
  char *c_str()const noexcept{
    return &start;
  }

  operator const char *()const noexcept{
    return &start;
  }

  /** @returns index of first inace of @param to seek starting the check at @param from. ~0 if nothing found. */
  unsigned nextChar(unsigned from, char toseek) const noexcept;
protected:
  /** pointer to char storage at given index. If index is out of range then first char is pointed at, hopefully making the bug obvious.  */
  char *at(unsigned index)const noexcept{
    return &(c_str()[index<allocated?index:0]);
  }
}; // class TextBlock

/** a view into a TextBlock, but neither owning nor owned by it. */
class TextChunk {
public:
  const TextBlock &block;
  Span span;
  TextChunk(const TextBlock &block);
  /** @returns a  TextBlock which covers the span */
  operator TextBlock() const noexcept;
  /** moves the view window along the block to start with next intsance of @param comma */
  void next(char comma);
};
