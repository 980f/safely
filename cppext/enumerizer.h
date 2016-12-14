#ifndef ENUMERIZER_H
#define ENUMERIZER_H

#include "eztypes.h" //countof()
#include "buffer.h"
#include <textkey.h>
//#include "note.h"
/** sketch of a textifier for an enum, not closely coupled at the moment.*/
class Enumerizer : public Indexer <TextKey> {
public:
  Enumerizer(const char **tokenSet, int numTokens);
  Enumerizer(const Enumerizer &other);
  TextKey token(unsigned ordinal) const;
  int ordinal(TextKey token) /*const */;
};

//this goes into a header file
#define ENUM(name) name ## _Text
#define DECLARE_ENUM(name) extern Enumerizer name ## _Text;

//the following should go only in the CPP file creating the symbols:
#define BEGIN_ENUM(name)   const char *name ## _Tokens[] = {

#define END_ENUM(name)  }; \
  Enumerizer name ## _Text InitStep(20000) (name ## _Tokens, sizeof(name ## _Tokens));

#endif // ENUMERIZER_H
