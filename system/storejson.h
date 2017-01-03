#ifndef STOREJSON_H
#define STOREJSON_H

#include "storable.h"
#include "charscanner.h"

/** parse relatively small blocks of json code.
 * This class is a factory for Storable's.  */
class StoredJSONparser {
  CharScanner token;//(buffer,length);
  CharScanner lookahead;//(buffer,length);
  Storable *parent;

  /** @returns new Storable, caller must add to wad  */
  Storable *parseChild(Storable *parent);
public:
  /** parse a block of text into a child of the given node. */
  StoredJSONparser(const CharScanner &loaded,Storable *parent);

  /** process the block */
  bool parse( );

public: //stats
  /**number of values */
  unsigned totalNodes = 0;

  /**number of terminal values */
  unsigned totalScalar = 0;

  /** greatest depth of nesting */
  unsigned maxDepth = 0;

  /** number of unmatched braces at end of parsing */
  unsigned nested = 0;

private:
  TextKey terminateField();
  Storable *makeChild(Storable *parent);
  Storable *makeNamelessChild(Storable *parent);
}; // class JSONparser

#endif // STOREJSON_H
