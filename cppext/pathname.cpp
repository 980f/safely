#include "pathname.h"

Pathname::Pathname(char slasher, Indexer<char> &other) :
  CharFormatter(other.internalBuffer(),other.allocated() - 1),
  seperator(slasher){
  other.clearUnused();
}
