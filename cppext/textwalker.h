#ifndef TEXTWALKER_H
#define TEXTWALKER_H

#include "sequence.h"
#include "textchunk.h"


/** parse a chunk of text, without altering the content herein */
class TextWalker: public Sequence<TextBlock>{
protected:
  TextChunk source;
  //hasNext will have found the next block, so we retain that instead of doing it twice..
  TextBlock lookahead;
  /** to distinguish  'haven't looked' from 'ran out of things to look for' */
  bool done;
public:
  TextWalker(const TextBlock &source);
  virtual ~TextWalker()=default;//stifle a warning
protected:
  //called by next, updates lookahead and source.span.
  virtual void seek()=0;
  // Sequence interface
public:
  virtual bool hasNext() const override;
  virtual TextBlock &next() override;
  virtual void skip(unsigned int qty) override;

};

class SimpleTextWalker:public TextWalker {
  char comma;
public:
  SimpleTextWalker(const TextBlock &source,char comma);
  void seek();
};

#endif // TEXTWALKER_H
