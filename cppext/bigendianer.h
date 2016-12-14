#ifndef BIGENDIANER_H
#define BIGENDIANER_H

#include "eztypes.h" //sized ints
#include "buffer.h" //for indexing access to data block

class BigEndianer : public Indexer <u8> {
public:
  BigEndianer(u8 * allocation, int length) : Indexer <u8> (allocation, length){}
  BigEndianer(Indexer <u8> other, bool justContent=true, int clip=0):Indexer<u8>(other, justContent , clip ){}

  int getU16(void){
    if(stillHas(2)) {
      u8 high = next();
      u8 low = next();
      return high << 8 | low;
    } else {
      return -1;
    }
  }

  int getI16(void){
    if(stillHas(2)) {
      s8 high = next();
      u8 low = next();
      return high << 8 | low;
    } else {
      return -1;
    }
  }

  void hilo(u16 datum){
    if(stillHas(2)) {
      next() = datum >> 8;
      next() = datum;
    } else {
      //not using exceptions ....
    }
  }

  u32 getu32(void);
  double getFloat(void);
  void put(u32 value);
  void put(double value);//will convert to 32 bit float.

  void insertVersion(void);
};
#endif // BIGENDIANER_H
