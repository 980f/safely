#ifndef STOREJSON_H
#define STOREJSON_H

#include "storable.h"
#include "charformatter.h"
/** parse relatively small blocks of json code. the caller will have to allocate */
class StoreJSON
{
public:
  StoreJSON();
  /** parse a block of text into a child of the given node. */
  void parse(char *buffer, int length, Storable &parent);
  /** @returns whether printing stopped short due to lack of room.
  This seems limiting, but would you really want to send a block of JSON if you weren't sure that it was going to be complete?
*/
  bool print(CharFormatter &printer, Storable &node);
};

#endif // STOREJSON_H
