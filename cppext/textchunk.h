#include "halfopen.h"

#pragma once

class TextBlock {
  friend class TextChunk;
public:
  char &start;
  const unsigned allocated;
  const bool amOwner;

  TextBlock(char *start,unsigned allocated,bool amOwner = false);
  ~TextBlock();
  TextBlock(TextBlock &&other);
  TextBlock(const TextBlock &other);

  int cmp(const char *other) const noexcept;

  bool operator==(const char *other) const noexcept;

  char operator [](unsigned index)const noexcept{
    return index<allocated?(&start)[index]:0;
  }

  unsigned nextChar(unsigned from, char toseek) const noexcept;
protected:
  char *at(unsigned index)const noexcept{
    return &((&start)[index]);
  }
}; // class TextBlock

class TextChunk {
public:
  TextBlock &block;
  Span span;
  TextChunk(TextBlock &block);
  operator TextBlock() const noexcept;

  void next(char comma);
};
