#ifndef ENUMERIZER_H
#define ENUMERIZER_H

#include "textkey.h"
#include "buffer.h"

/** use an indexer to implement a text<->index mapping */
class Enumerizer : public Indexer<TextKey> {
public:
  Enumerizer(TextKey tokenSet[], int quantity);
  Enumerizer(const Enumerizer &other);
  TextKey token(unsigned ordinal) const;
  unsigned ordinal(TextKey token) /*const */;
};


// *INDENT-OFF*   unbalanced braces in the macros piss off uncrustify
//this goes into a header file
#define ENUM(name) name ## _Text
#define DECLARE_ENUM(name) extern Enumerizer name ## _Text;

//the following should go only in the CPP file creating the symbols:
#define BEGIN_ENUM(name)   const char *name ## _Tokens[] = {

#define END_ENUM(name)  }; \
  Enumerizer name ## _Text InitStep(20000) (name ## _Tokens, sizeof(name ## _Tokens));

#endif // ENUMERIZER_H
