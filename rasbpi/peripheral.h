#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "memorymapper.h"
class Peripheral{
public://until we wrap with block accessors
  /*each peripheral has a block of address space. We cannot directly address it, we use a memory mapper object by which the OS gives us an address that will modify the physical one. */
  Mapped<unsigned> reg;//volatile here was very syntactically expensive, and most of the time either unnecessary or we really wanted atomic update rather than averting caching a value. Only in a polling loop would the 'volatile' matter.
public:
  /** blockOffset is a byte address, blockLength is number of 32 bit registers, not number of bytes/ */
  Peripheral(unsigned blockOffset, unsigned blockLength);
};

unsigned const BCM_PASSWD  =(0x5A<<24);

#endif // PERIPHERAL_H
