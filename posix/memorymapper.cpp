#include "memorymapper.h"
#include "fcntlflags.h"
#include <sys/mman.h>

MemoryMapper *MemoryMapper::Mmap=nullptr;


MemoryMapper::MemoryMapper(){
  fd.open("safe-memory", O_RDWR | O_SYNC);
}

void *MemoryMapper::map(unsigned addr, unsigned len){//initMapMem
  //for mbMapMem: PROT_READ|PROT_WRITE, MAP_SHARED
  void *peeker=mmap(0, len, PROT_READ|PROT_WRITE|PROT_EXEC,  MAP_SHARED|MAP_LOCKED,  fd, addr);
  if(long(peeker)==~0){
    return nullptr;
  } else {
    return peeker;
  }
}

bool MemoryMapper::free(void *addr, unsigned size){
  // * 0 okay, -1 fail */
  return fd.ok(munmap(addr, size));
}

bool MemoryMapper::init(bool refresh){
  if(Mmap || refresh){
    delete Mmap;
    Mmap=nullptr;
  }
  Mmap=new MemoryMapper();
  return Mmap!=nullptr && Mmap->fd.isOpen();
}
