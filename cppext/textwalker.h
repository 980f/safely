#pragma once

#include "sequence.h"
#include "textchunk.h"

class TextWalker: public Sequence<TextBlock>{
protected:
  TextChunk source;
  TextBlock lookahead;
  bool done;
  TextWalker(const TextBlock &source);
  virtual ~TextWalker()=default;
  bool hasNext() override;
  void skip(unsigned int qty) override;

// Sequence interface
public:
  TextBlock &next() override;
  virtual void seek()=0;
};


///////////////
class SimpleTextWalker:public TextWalker{
public:
  SimpleTextWalker(const TextBlock &source, char comma=',');
  char comma;
  void seek()override;
};
