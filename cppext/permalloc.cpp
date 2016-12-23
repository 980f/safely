#include "permalloc.h"

/**
heapless systems are hard to reconcile with those which do have one.
this module allows sharing of code under the strict conditions that
one never delete any object that uses this non-heap heap.
Usually that means only static and local-to-main objects can be permalloc users.
*/


#ifndef PermAllocPool
#error PermAllocPool must be defined in your build manager.
#endif

//some of this code is used on a PC wherein related objects are created on heap, so on PC we use the actual heap.
#if PermAllocPool==0
#include <stdlib.h>
void *permAlloc(unsigned quantity,unsigned sizeofone){
  return calloc(quantity,sizeofone);
}
void permFree(void *calloced){
  free(calloced);
}
#else
void permFree(void *calloced){
  //do nothing except maybe wtf.
}

static char pool[PermAllocPool] __attribute__ ((aligned (4)));//we rely upon the compiler and linker aligning this array.
static unsigned permanentlyAllocated(0);// which is also 'next to allocate'
void *permAlloc(unsigned quantity,unsigned sizeofone){
  int start=permanentlyAllocated;
  int aligner=permanentlyAllocated%sizeofone;//need to roundup to next multiple of given thing, some items need to be aligned in memory or we get an UNALIGNED fault.
  if(aligner){
    aligner=sizeofone-aligner;//e.g. at byte 3 when one is 4 means we have to add 1 byte
  }
  permanentlyAllocated+=quantity*sizeofone+aligner;

  if(permanentlyAllocated<=sizeof(pool)){
    return &pool[start+aligner];
  } else {
    permanentlyAllocated=start;
    return 0;//will blow hard! check: is that true even if bootup is into ram?
  }
}
#endif
