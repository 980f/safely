#ifndef URLTEXT_H
#define URLTEXT_H

#include "cstr.h"
#include "countdown.h"
#include "charscanner.h"
#include "char.h"

namespace UrlText {
/** @returns the length the string will have when special chars are replaced with %xx sequences. */
unsigned encodedLength(const char* it);

/** @returns the length the string will have when %xx sequences are replaced with special chars . */
unsigned decodedLength(const char* it);

bool mustEscape(Char ch);

/** expands string into URL compatible form.  */
class Expander: Indexer<const char>{
  CountDown hidgetting;
  /** if hidget is not zero then this is char being %encoded */
  char hidgets[3];
public:
  bool hasNext() const override {
    return hidgetting>0 || Indexer::hasNext();
  }
  const char &next() override;
public:
  Expander(const char *ptr, unsigned length);
  virtual ~Expander();
};

/** expands string into URL compatible form.  */
class Decoder: Indexer<char>{
  CountDown hidgetting;
  /** if hidget is not zero then this is char being %encoded */
  char packer;
public:
  bool hasRoom() const {
    return !hidgetting.done() || Indexer::hasNext();
  }
  bool push(char next) ;
public:
  Decoder(char *ptr, unsigned length);
  virtual ~Decoder();
};


}
#endif // URLTEXT_H
