#include "halfopen.h"

#pragma once

class TextBlock {
  friend class TextChunk;

public:
  char &start;
  const unsigned allocated;
  const bool amOwner;

  TextBlock(char *start, unsigned allocated, bool amOwner = false);

  ~TextBlock();

  TextBlock(TextBlock &&other);

  TextBlock(const TextBlock &other);

  int cmp(const char *other) const noexcept;

  bool operator==(const char *other) const noexcept;

  /** @ returns null if index is not valid, else the char at that index.*/
  char operator [](unsigned index) const noexcept {
    return index < allocated ? (&start)[index] : 0;
  }

  /** you are on your own if you use this. */
  char *c_str() const noexcept {
    return &start;
  }

  operator const char *() const noexcept {
    return &start;
  }

  /** @returns index of next instance of @param toseek starting search at @param from
   * csv usage pattern: start=block.nextChar(0,','); end = block.nextChar(++start,',');
   */
  unsigned nextChar(unsigned from, char toseek) const noexcept;

protected:
  /** unsafe pointer, only used by routines that first check the index. */
  char *at(unsigned index) const noexcept {
    return &c_str()[index];
  }
}; // class TextBlock

/** core of a parser looking into an immutable block of text. */
class TextChunk {
public:
  const TextBlock &block;
  Span span;

  TextChunk(const TextBlock &block);

  /** @returns a  TextBlock which covers the span */
  operator TextBlock() const noexcept;

  void next(char comma);
};
