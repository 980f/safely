#include <string.h>
#include "minimath.h"

/**
parts of C startup routine of microcontroller were exposed for use application use, 
but then that code was used on a linux system so we need to map the C-startup stuff to standard library stuff.

some other platform optimizations seem to have slipped in ...
*/

//todo:2 figure out some #define's to switch between os's
extern "C" {
  void copyObject(const void *source, void *target, u32 length) {
    memcpy(target, source, length);
  }

  void fillObject(void *target, u32 length, u8 fill) {
    memset(target, fill, length);
  }

  //? from minimath.h?	
  u32 muldivide(u32 rawcount, u32 referenceCounts, u32 recentCounts){
    return rate(rawcount*referenceCounts,recentCounts);
  }

}
