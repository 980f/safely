#include "bigendianer.h"

BigEndianer::BigEndianer(uint8_t *allocation, unsigned length) : Indexer(allocation, length){
}

BigEndianer::BigEndianer(Indexer other, int clip) : Indexer(other, clip ){
}


unsigned BigEndianer::getu16(){
  if(stillHas(2)) {
    uint8_t high = next();
    uint8_t low = next();
    return high << 8 | low;
  } else {
    return -1;
  }
}

int BigEndianer::getI16(){
  if(stillHas(2)) {
    uint8_t high = next();
    uint8_t low = next();
    return high << 8 | low;
  } else {
    return -1;
  }
}

void BigEndianer::hilo(uint16_t datum){
  if(stillHas(2)) {
    next() = datum >> 8;
    next() = datum;
  } else {
    //not using exceptions ....
  }
}

uint32_t BigEndianer::getu32(){
  uint32_t packed = getu16() << 16;
  packed |= getu16();
  return packed;
}

double BigEndianer::getFloat(){
  uint32_t packed = getu32();
  return pun(float,packed);
}

void BigEndianer::put(uint32_t value){
  hilo(value >> 16);
  hilo(value);
}

void BigEndianer::put(double value){
  float arf = value;
  put(pun(uint32_t,arf));
}

//end of file
