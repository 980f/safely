#ifndef PATHNAME_H
#define PATHNAME_H


#include "charformatter.h"

#include "eztypes.h"  //fixed elements of paths

/** build a segment name in an overflow protected buffer, such as for a filename, but that will be an embellishment of this.
 *  this class does not allocate data, you give it via pointer and allocation size */
class Pathname : public CharFormatter {
  const char seperator;

public:
  Pathname(char slasher,Indexer<char> &other);
  virtual ~Pathname();
  void addNode(Cstr node);
};

#endif // PATHNAME_H
