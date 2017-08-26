#ifndef MANAGEDBLOCK_H
#define MANAGEDBLOCK_H

#include "block.h" //extends functionality of this
#include "chain.h"
#include "buffer.h"
/** a block of memory which hands out constrained pointers, and when deleted will cripple those pointers */

template <typename Content> class ManagedBlock: public Block<Content> {
  Chain<Indexer<Content>> pointers;
public:
  /** dangerous constructor, trusting caller knows the length of the buffer */
  ManagedBlock(unsigned length,Content *buffer,bool ownit=false):
    Block<Content>(length,buffer,ownit){
    //#nada
  }
  /** creates one*/
  ManagedBlock(unsigned length):Block<Content>(length){
   //#nada
  }

  ~ManagedBlock(){
    //automatically calls base destructor
    ChainScanner<Content> killer=ChainScanner<Content>(pointers);
    while(killer.hasNext()){
      killer.next().clear();
    }
  }
};

#endif // MANAGEDBLOCK_H
