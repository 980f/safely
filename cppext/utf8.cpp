#include "utf8.h"
#include "bitwise.h" //bitfields

#include "ignoresignwarnings.h"

/** first byte tells you how many follow, number of leadings ones -2 (FE and FF are both 5)
 *  subsequent bytes start with 0b10xx xxxx 80..BF, which are not legal single byte chars.
 */
unsigned UTF8::numFollowers() const noexcept {
  if(u8(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here, C0 and C1  are not specfied so lump them in as well
    return 0;   //7 bits    128
  }
  //unrolled loop:
  if(u8(raw) < 0xE0) {
    return 1;   //5 bits,  6 bits 2k
  }
  if(u8(raw) < 0xF0) {
    return 2;   //4bits, 6, 6    64k
  }
  if(u8(raw) < 0xF8) {
    return 3;   //3 bits, 6,6,6  2M
  }
  if(u8(raw) < 0xFC) {//not yet used
    return 4;
  }
  return 5; //not yet used
}

void UTF8::firstBits(Unichar &uch,unsigned nf) const noexcept {
  if(u8(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here, C0 and C1  are not specfied so lump them in as well
    //we can't trust numFollowers if this byte isn't multiByte
    uch=0;   //illegal argument
  } else {
    if(nf==~0){//unknown, so go compute
      nf=numFollowers();
    }
    //need to keep 6-nf bits
    uch=fieldMask(6-nf)&u8(raw);
  }
}

void UTF8::moreBits(Unichar &uch) const noexcept {
  uch<<=6;
  uch |= fieldMask(6)&u8(raw);
}

 void UTF8::pad(Unichar &uch, unsigned followers) noexcept{
   uch<<=(6*followers);
 }

unsigned UTF8::numFollowers(u32 unichar) noexcept{
  if(unichar < (1U << 7)) {//quick exit for ascii
    return 0;
  }
  for(int f=1;f<6;++f){
    unsigned bits=(6*f) + (6-f);
    if(unichar<(1U << bits)) {
      return f;
    }
  }
  //it appears unicode is likely to stop at 2G characters
  return 0;//not yet implementing invalid extensions.
} // UTF8::numFollowers

u8 UTF8::firstByte(u32 unichar, unsigned followers) noexcept{
  if(followers) {
    u8 prefix(0xFC);
    prefix <<= (5 - followers);//1->C0, 2->E0 3->F0 4->F8
    unichar >>= (6 * followers);
    return prefix |= unichar;
  } else {
    return static_cast<u8>(unichar);//only the one byte needed.
  }
}

u8 UTF8::nextByte(u32 unichar, unsigned followers) noexcept{
  unsigned shift = 6 * followers;
  unichar >>= shift;
  unichar &= fieldMask(6);
  unichar |= (1 << 7);
  return static_cast<u8>(unichar);//# truncate to 8 bits.
}

char UTF8::hexNibble(Unichar uch, unsigned sb) noexcept {
  u8 nib= 15&(uch>>(sb*4)); //push to low nib
  return nib>9? 'A'+nib-10: '0'+nib;
}

