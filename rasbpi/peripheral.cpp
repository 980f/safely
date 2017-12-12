#include "peripheral.h"

static unsigned PiSpace=0x7e000000;
Peripheral::Peripheral(unsigned blockOffset, unsigned blockLength):
  reg(PiSpace|blockOffset,blockLength){
  //we could cache some items here such as validity of the mapping.
}
