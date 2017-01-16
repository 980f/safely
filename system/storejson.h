#ifndef STOREJSON_H
#define STOREJSON_H

//for arguments:
#include "storable.h"
#include "buffer.h"

//for internals:
#include "extremer.h"
#include "utf8.h"

#include "pushedjsonparser.h" //implemented with microcontroller-viable parser

/** parse relatively small blocks of json code.
 * This class is a factory for Storable's.
This does not have an iostream interface as that sucks in scads of code and is not likely to be useful.
If receiving json one should receive the whole block before starting to parse- no good can come from partial objects.
If you must work with a stream with no out of band framing then you can use a pushedjsonparser to recognize the end of a top level item, accumulating the chars and the name:value offset that the parser puts out. You can then rummage through the block you accumulated and quickly construct objects.
  */
class StoredJSONparser {
public:
  /** parse a block of text into a child of the given node. */
  StoredJSONparser(Indexer<const char> &loaded,Storable *root);

  /** process the block */
  bool parse(Storable *&root );

public: //stats
  JsonStats s;

private:
  Indexer<const char> data;
  Storable *root;
  PushedJSON::Parser parser;

  /** @returns whether there are more children, for recursively parsing wads. */
  bool parseChild(Storable *parent);
  //code fragment
  Storable *insertNewChild(Storable *parent, TextKey name);
  /** @param evenIfempty might also be called isWad */
  Storable *assembleItem(Storable *parent,bool evenIfEmpty=false);
}; // class JSONparser


#endif // STOREJSON_H
