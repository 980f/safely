#ifndef MPSSE_H
#define MPSSE_H "(C) Andrew L. Heilveil, 2017"

#include "charscanner.h"

//created in '4aai' repo.

/** a class for building up an MPSSE string of operations. The actual storage is allocated by the caller, this just makes sure that it doesn't run off the end of that.*/
class MPSSE : public ByteScanner{
public:
  /** this constructor wraps a reference into the available data of the buffer allocated, typically pass it peek()/freespace() */
  MPSSE(u8 *buffer, unsigned sizeofBuffer);

  /** TCK/SK period = 6MHz / (1 + @param divisor) for original series, need a flag and more logic to support the H series parts. */
  void setClockDivisor(unsigned divisor);

  /** adds command to emit up to 8 bits serially */
  void sendBitsMsf(unsigned numberBits,u8 data,bool edginess);

  /** adds command codes to have @param numBYTES returned, note that some commands talk of bits */
  void fetchBytes(unsigned numBYTES);

  /** adds instruction and data to shift out */
  void shiftoutBytes(Indexer<u8> blob);

  /** you must follow this with appending quantity bytes to message */
  void shiftoutBytes(unsigned quantity);

  /** add command codes to get gpio byte, if both then @param ls will preceed @param ms in the response data. */
  unsigned fetchGpio(bool ls,bool ms);

  /** set one of the gpio bytes, @param highbits selects which. @param dirbits defines which pins are outputs (1=output) @param value is the pattern of outputs. */
  void setGpio(u8 value, u8 dirbits, bool highbits);
};

#endif // MPSSE_H
