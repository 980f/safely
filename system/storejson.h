#ifndef STOREJSON_H
#define STOREJSON_H

#include "storable.h"
#include "charscanner.h"
/** parse relatively small blocks of json code. the caller will have to allocate */
class JSONparser {
  CharScanner token;//(buffer,length);
  CharScanner lookahead;//(buffer,length);
  Storable &parent;
  void parse( );
public:
  /** parse a block of text into a child of the given node. */
  JSONparser(char *buffer, unsigned length,Storable &parent);

public: //stats
  /**number of values */
  unsigned totalNodes = 0;

  /**number of terminal values */
  unsigned totalScalar = 0;

  /** greatest depth of nesting */
  unsigned maxDepth = 0;

  /** number of unmatched braces at end of parsing */
  unsigned nested = 0;

}; // class JSONparser

#endif // STOREJSON_H
