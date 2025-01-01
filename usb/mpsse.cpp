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


MPSSE::MPSSE(uint8_t *buffer, unsigned sizeofBuffer):ByteScanner (buffer,sizeofBuffer){
  //#nada
}

void MPSSE::setClockDivisor(unsigned divisor){
  next() = 0x86;//Set CLK divisor
  putuint16_t(divisor);
}

void MPSSE::sendBitsMsf(unsigned numberBits, uint8_t data, bool edginess){
  next() = edginess?  uint8_t( BitWad<bitly,doOutput,ockNegative>::mask) : uint8_t( BitWad<bitly,doOutput>::mask);
  next() = uint8_t(numberBits-1);
  next() = data;
}

void MPSSE::fetchBytes(unsigned numBYTES){
  next() = BitWad<doInput>::mask;
  putuint16_t(numBYTES - 1);
}


void MPSSE::shiftoutBytes(unsigned quantity){
  if(quantity>0){
    next() = BitWad<doOutput>::mask;
    putuint16_t(quantity - 1);//length -1
  }
}

void MPSSE::shiftoutBytes(Indexer<uint8_t> blob){
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

void MPSSE::setGpio(uint8_t value, uint8_t dirbits,bool highbits){
  next() = highbits? uint8_t(BitWad<Configure,GpioMsbyte>::mask):uint8_t(BitWad<Configure>::mask);
  next() = value;
  next() = dirbits;
}
