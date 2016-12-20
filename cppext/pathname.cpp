#include "pathname.h"

Pathname::Pathname(Indexer<char> &other,char slasher) :
  CharFormatter(other.internalBuffer(),other.allocated() - 1),
  seperator(slasher){
  other.clearUnused();
}
