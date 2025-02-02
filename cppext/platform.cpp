#include <cstring>
#include "minimath.h"

/**

parts of C startup routine of microcontroller were exposed for use application use,
but then that code was used on a linux system so we need to map the C-startup stuff to standard library stuff.

some other platform optimizations seem to have slipped in ...
*/

//todo:2 figure out some #define's to switch between os's
  void copyObject(const void *source, void *target, uint32_t length) {
    memmove(target, source, length);
  }

  void fillObject(void *target, uint32_t length, uint8_t fill) {
    memset(target, fill, length);
  }

  //? from minimath.h?
  uint32_t muldivide(uint32_t rawcount, uint32_t referenceCounts, uint32_t recentCounts){
    return rate(rawcount*referenceCounts,recentCounts);
  }

  void nanoSpin(unsigned clocks){
    while(clocks-->0); //todo: platform specific code
  }
