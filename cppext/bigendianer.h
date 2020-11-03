#ifndef BIGENDIANER_H
#define BIGENDIANER_H "(C) 2012 Andrew Heilveil (github/980f) "

#include "eztypes.h" //sized ints
#include "buffer.h" //for indexing access to data block


/** utility for packing and unpacking binary values in byte order that might not match the platform order */
class BigEndianer : public Indexer<u8> {
public:
  BigEndianer(u8 * allocation, unsigned length);

  BigEndianer(Indexer<u8> other, int clip = 0);
  virtual ~BigEndianer();
  unsigned getU16(void);
  int getI16(void);
  void hilo(u16 datum);
  u32 getu32(void);
  double getFloat(void);
  void put(u32 value);
  /** put 32 bit ieee format number into byte array. You might get a Nan for extreme @param value */
  void put(double value);//will convert to 32 bit float.

}; // class BigEndianer
#endif // BIGENDIANER_H
