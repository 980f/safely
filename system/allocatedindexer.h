#ifndef ALLOCATEDINDEXER_H
#define ALLOCATEDINDEXER_H "(C) Andrew L. Heilveil, 2017"

#include "buffer.h"
#include <cstdlib>

/** dynamically allocated buffer, with pointer knowledge.
@deprecated untested */
template<typename Content> class AllocatedIndexer : public Indexer<Content>{
public:
  using Indexer<Content>::length;
  using Indexer<Content>::buffer;

  explicit AllocatedIndexer(unsigned quanta) :
    Indexer<Content>(calloc(quanta,sizeof(Content)),quanta*sizeof(Content)){
  }

  bool isLegitimate()const{
    return length>0 && buffer!=nullptr;
  }

  virtual ~ AllocatedIndexer (){
    free(buffer);
  }
};


#endif // ALLOCATEDINDEXER_H
