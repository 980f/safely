#include "bigendianer.h"

u32 BigEndianer::getu32(void){
  u32 packed=getU16()<<16;
  packed|=getU16();
  return packed;
}

double BigEndianer::getFloat(void){
  u32 packed=getu32();
  return pun(float,packed);
}

void BigEndianer::put(u32 value){
  hilo(value>>16);
  hilo(value);
}

//void BigEndianer::put(float value){
//  put(pun(u32,value));
//}

void BigEndianer::put(double value){
  float arf=value;
  put(pun(u32,arf));
}

//void BigEndianer::insertVersion(void){
//  hilo(
//#include "svnrevision.txt"
//      );
//}
//end of file
