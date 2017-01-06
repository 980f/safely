#ifndef URLTEXT_H
#define URLTEXT_H

#include "cstr.h"
#include "cheaptricks.h"
#include "charscanner.h"
#include "char.h"

/** expands string into URL compatible form.  */
class UrlText: Indexer<const char>{

  CountDown hidget;
  /** if hidget is not zero then this is char being %encoded */
  char hidgets[3];

public:
  bool hasNext() const override {
    return hidget>0 || Indexer::hasNext();
  }

  const char &next() override;
public:
  UrlText(const char *ptr, unsigned length);
  virtual ~UrlText();
  /** @returns the length the string will have when special chars are replaced with %xx sequences. */
  static unsigned encodedLength(const char* it);
};

#endif // URLTEXT_H
