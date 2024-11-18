#pragma once
#define MEMORYMAPPER_H "(C) Andrew L. Heilveil, 2017"

#include "fildes.h"


/** wrapper for /dev/gpiomem access to memory via memmap of file.
rather than have a #define herein for debug the user should create a regular file for simulation and link to /dev/gpiomem for rpi real operation.
This lets you record (attempted) writes to the area of interest.
The Mapped class lets you not do even that, on failure to memory map it allocates a block of ram.
*/
class MemoryMapper {
  Fildes fd;
  MemoryMapper ();
public:
  /** @returns pointer to block of bytes at offset @param addr of @parm len bytes. nullptr if couldn't be done, see getError() for detail.
wraps mmap sysmtem call. */
  void *map(unsigned addr, unsigned len);
  /** could be static, but we don't want to call it without the context of having accessed map, and it gives us a place to save error codes. wraps munmap system call. */
  bool free(volatile void *addr, unsigned size);
  /** error code from most recent failure. successes do NOT erase that code. */
  int getError();
  /** creates shared one. If this fails your application can't access whatever it is. The Mapped class has aids for dealing with this. */
  static bool init(bool refresh=false);
  static MemoryMapper *Mmap;
 /** @returns whether /dev/gpiomem has been successfully opened. */
  static bool isOperational();
};


/** wrapper class to ensure we unmap when we drop the pointer.
 It also will allocate a block of actual ram for when the desired block cannot be mmap'd. This averts null pointers and in some cases can let you see what you would write to memory if you could access it. */
template <typename Any> class Mapped {
  /** 0th Any */
  Any *ptr;
  /** number of Any's, not number of bytes*/
  unsigned quantity;
  /** if module is not init we allocated some ram */
  bool simulated;
public:
  Mapped(unsigned physical,unsigned quantity):ptr(nullptr),quantity(quantity){
    if(MemoryMapper::init()){
      ptr= reinterpret_cast<Any *>(MemoryMapper::Mmap->map(physical,quantity*sizeof (Any)));
      simulated=false;
    } else {
      ptr=new Any[quantity];
      simulated=true;
    }
  }

  ~Mapped(){
    if(simulated){
      delete [] ptr;
    } else {
      MemoryMapper::Mmap->free(ptr,quantity*sizeof (Any));
    }
    ptr=nullptr;//in case someone explicitly calls delete, or uses a stale reference
  }

  /** call this to disable access for the remainder of the program run.
   * this is useful when peripheral access kills your program but isn't essential but you also don't want to configure it into disablement or comment out more than one line of code.
*/
  void fakeit(){
    if(!simulated){
      MemoryMapper::Mmap->free(ptr,quantity*sizeof (Any));
      ptr=new Any[quantity];
      for(unsigned clears=quantity;clears-->0;){//4debug
        ptr[clears]=0;
      }
      simulated=true;
    }
  }

  /** @returns whether this block of memory is actually mapped to something vs just being ram. */
  bool isReal() const noexcept {
    return this!=nullptr && !simulated;
  }

private:
  void operator =(Any *other)=delete;//do not allow copying
  Mapped(&&Mapped thing)=delete;
public:
  /** bad index results in whacking the 0th element, hopefully you will notice that. */
  Any &operator [](unsigned index)const {
    return ptr[index<quantity?index:0];
  }
  //do not bother with inc's
};
