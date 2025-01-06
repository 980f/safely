//"(C) Andrew L. Heilveil, 2017"
#include "memorymapper.h"
#include "fcntlflags.h"
#include <sys/mman.h>
//all errors are reported on via modules this calls. #include "errno.h"

MemoryMapper *MemoryMapper::Mmap = nullptr;


MemoryMapper::MemoryMapper(): fd("MemoryMapper") {
  fd.open("/dev/gpiomem", O_RDWR | O_SYNC);
}

void *MemoryMapper::map(unsigned addr, unsigned len) {
  //MAP_SHARED may be gratuitous, for actual files it is used to push the data to the file
  //MAP_LOCKED may be gratuitous, for actual files it triggers read ahead
  void *peeker = nullptr;

  if (fd.okValue(peeker, mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, addr))) {
    return peeker;
  } else {
    return nullptr;
  }
}

bool MemoryMapper::free(volatile void *addr, unsigned size) {
  return fd.ok(munmap(const_cast<void *>(addr), size));
}

int MemoryMapper::getError() {
  if (fd.isOk()) {
    if (fd.isOpen()) {
      return 0;
    } else {
      return ENOMEM;
    }
  } else {
    return fd.errornumber;
  }
}

bool MemoryMapper::init(bool refresh) {
  if (refresh) {
    Obliterate(Mmap);
  }
  if (Mmap == nullptr) {
    Mmap = new MemoryMapper();
  }
  return isOperational();
}

bool MemoryMapper::isOperational() {
  return Mmap != nullptr && Mmap->fd.isOpen();
}
