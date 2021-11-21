//"(C) Andrew L. Heilveil, 2017"
#include "mpsse.h"
#include "bitbanger.h"

//some of this might be specific to the first use of this lib rather than generic.
/** bit position numbers (not masks) */
enum OpBits {
  ockNegative=0, //-ve CLK on write
  bitly,         //bit mode = 1 else byte mode
  ickNegative,   //-ve CLK on read
  LSBfirst,      // = 1 else MSB first

  doOutput,
  doInput,       //Do read TDO
  enableTMS,
  //bit 7 low

  Configure=7,
    GpioRead=0,
    GpioMsbyte,

  Loopback=2,
    LoopOpen=0,
};


MPSSE::MPSSE(u8 *buffer, unsigned sizeofBuffer):ByteScanner (buffer,sizeofBuffer){
  //#nada
}

void MPSSE::setClockDivisor(unsigned divisor){
  next() = 0x86;//Set CLK divisor
  putU16(divisor);
}

void MPSSE::sendBitsMsf(unsigned numberBits, u8 data, bool edginess){
  next() = edginess?  u8( BitWad<bitly,doOutput,ockNegative>::mask) : u8( BitWad<bitly,doOutput>::mask);
  next() = u8(numberBits-1);
  next() = data;
}

void MPSSE::fetchBytes(unsigned numBYTES){
  next() = BitWad<doInput>::mask;
  putU16(numBYTES - 1);
}


void MPSSE::shiftoutBytes(unsigned quantity){
  if(quantity>0){
    next() = BitWad<doOutput>::mask;
    putU16(quantity - 1);//length -1
  }
}

void MPSSE::shiftoutBytes(Indexer<u8> blob){
  shiftoutBytes(blob.allocated());
  appendAll(blob);
}

unsigned MPSSE::fetchGpio(bool ls, bool ms){
  unsigned resplength=0;
  if(ls){
    ++resplength;
    next()=BitWad<Configure,GpioRead>::mask;
  }
  if(ms){
    ++resplength;
    next()=BitWad<Configure,GpioRead,GpioMsbyte>::mask;
  }
  return resplength;
}

void MPSSE::setGpio(u8 value, u8 dirbits,bool highbits){
  next() = highbits? u8(BitWad<Configure,GpioMsbyte>::mask):u8(BitWad<Configure>::mask);
  next() = value;
  next() = dirbits;
}
