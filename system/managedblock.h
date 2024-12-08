#ifndef MANAGEDBLOCK_H
#define MANAGEDBLOCK_H

#include "block.h" //extends functionality of this
#include "binder.h"
#include "buffer.h"

/** a block of memory which hands out constrained pointers, and when deleted will cripple those pointers.*/
template <typename Content> class ManagedBlock: public Block<Content> {
  Binder<Indexer<Content>> pointers;
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
    ChainScanner<Content> killer=ChainScanner<Content>(pointers);//don't use getPointer
    while(killer.hasNext()){
      killer.next().clear();
    }
  }
  Indexer<Content>& getPointer() {
    auto newone= ChainScanner<Content>(Block::buffer,Block::length);
    pointers.append(newone);
    return newone;
  }

  void releasePointer(Indexer<Content>&expired) {
    pointers.remove(expired);
  }

};

#endif // MANAGEDBLOCK_H
