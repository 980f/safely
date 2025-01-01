#pragma once

#include <cstdint>
#include "buffer.h" //for indexing access to data block

/** utility for packing and unpacking binary values in byte order that might not match the platform order */
class BigEndianer : public Indexer<uint8_t> {
public:
  BigEndianer(uint8_t *allocation, unsigned length);

  BigEndianer(Indexer other, int clip = 0);

  virtual ~BigEndianer() = default;

  unsigned getu16();

  int getI16();

  void hilo(uint16_t datum);

  uint32_t getu32();

  double getFloat();

  void put(uint32_t value);

  /** put 32 bit ieee format number into byte array. You might get a Nan for extreme @param value */
  void put(double value); //will convert to 32 bit float.
}; // class BigEndianer
