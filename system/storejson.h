#ifndef STOREJSON_H
#define STOREJSON_H

//for arguments:
#include "storable.h"
#include "charscanner.h"

//for internals:
#include "extremer.h"
#include "utf8.h"

#include "pushedjsonparser.h" //trying to implement with microcontroller parser
/** parse relatively small blocks of json code.
 * This class is a factory for Storable's.  */
class StoredJSONparser {
public:
  /** parse a block of text into a child of the given node. */
  StoredJSONparser(const CharScanner &loaded,Storable *root);

  /** process the block */
  bool parse( );

public: //stats
  /**number of values */
  unsigned totalNodes = 0;

  /**number of terminal values */
  unsigned totalScalar = 0;

  /** greatest depth of nesting */
  SimpleExtremer<unsigned> maxDepth;

  /** number of unmatched braces at end of parsing */
  unsigned nested = 0;

private: //to partition what would be an enormous switch with redundant cases we make a bunch of stateinfo members. if I could get the hang of functions defined within functions ....
  CharScanner data;
  Storable *root;
  PushedJSON::Parser parser;

  /** @returns whether there are more children */
  bool parseChild(Storable *parent);

  Storable *makeChild(Storable *parent);
  Storable *makeNamelessChild(Storable *parent);
  void setValue(Storable &nova);
  Storable *insertNewChild(Storable *parent, TextKey name);

  Storable *assembleItem(Storable *parent);
}; // class JSONparser

#endif // STOREJSON_H
