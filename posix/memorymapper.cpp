#include "memorymapper.h"
#include "fcntlflags.h"
#include <sys/mman.h>
#include "errno.h"

MemoryMapper *MemoryMapper::Mmap=nullptr;

//rather than have a compiler control create a regular file for simulation and link to /dev/gpiomem for rpi real operation.
MemoryMapper::MemoryMapper():fd("MemoryMapper"){
  fd.open("/dev/gpiomem", O_RDWR | O_SYNC);
}

void *MemoryMapper::map(unsigned addr, unsigned len){//initMapMem
  //for mbMapMem: PROT_READ|PROT_WRITE, MAP_SHARED
  //MAP_SHARED may be gratuitous, for actual files is used to push the data to the file
  //MAP_LOCKED may be gratuitous, for actual files is triggers read ahead
  void *peeker;//=mmap(0, len, PROT_READ|PROT_WRITE/*|PROT_EXEC*/,  MAP_SHARED|MAP_LOCKED,  fd, addr);

  if(fd.okValue(peeker, mmap(0, len, PROT_READ|PROT_WRITE/*|PROT_EXEC*/,  MAP_SHARED|MAP_LOCKED,  fd, addr))){
    return peeker;
  } else {
    return nullptr;
  }
}

bool MemoryMapper::free(void *addr, unsigned size){
  return fd.ok(munmap(addr, size));
}

int MemoryMapper::getError(){
  if (fd.isOk()){
    if(fd.isOpen()){
      return 0;
    } else {
      return ENOMEM;
    }
  } else {
    return fd.errornumber;
  }
}

bool MemoryMapper::init(bool refresh){
  if(Mmap || refresh){
    delete Mmap;
    Mmap=nullptr;
  }
  Mmap=new MemoryMapper();
  return isOperational();
}

bool MemoryMapper::isOperational(){
  return Mmap!=nullptr && Mmap->fd.isOpen();
}
