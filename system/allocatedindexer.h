#ifndef ALLOCATEDINDEXER_H
#define ALLOCATEDINDEXER_H

#include "buffer.h"
#include "stdlib.h"
/** dynamically allocated buffer, with pointer knowledge.
@deprecated untested */
template<typename Content> class AllocatedIndexer : public Indexer<Content>{
public:
  AllocatedIndexer (unsigned quanta):
    Indexer<Content>(calloc(quanta,sizeof(Content)),quanta*sizeof(Content)){
    if(quanta!=length){
      //andy can't program worth a damn.
    }
  }

  bool isLegitimate()const{
    return length>0 && buffer!=nullptr;
  }

  virtual ~ AllocatedIndexer (){
    free(buffer);
  }
};


#endif // ALLOCATEDINDEXER_H
