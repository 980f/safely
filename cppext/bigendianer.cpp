#include "bigendianer.h"

BigEndianer::BigEndianer(u8 *allocation, unsigned length) : Indexer<u8>(allocation, length){
}

BigEndianer::BigEndianer(Indexer<u8> other, int clip) : Indexer<u8>(other, clip ){
}

BigEndianer::~BigEndianer(){
  //exists to get rid of warning about multiple v-tables.
}

int BigEndianer::getU16(){
  if(stillHas(2)) {
    u8 high = next();
    u8 low = next();
    return high << 8 | low;
  } else {
    return -1;
  }
}

int BigEndianer::getI16(){
  if(stillHas(2)) {
    s8 high = next();
    u8 low = next();
    return high << 8 | low;
  } else {
    return -1;
  }
}

void BigEndianer::hilo(u16 datum){
  if(stillHas(2)) {
    next() = datum >> 8;
    next() = datum;
  } else {
    //not using exceptions ....
  }
}

u32 BigEndianer::getu32(void){
  u32 packed = getU16() << 16;
  packed |= getU16();
  return packed;
}

double BigEndianer::getFloat(void){
  u32 packed = getu32();
  return pun(float,packed);
}

void BigEndianer::put(u32 value){
  hilo(value >> 16);
  hilo(value);
}

void BigEndianer::put(double value){
  float arf = value;
  put(pun(u32,arf));
}

//end of file
