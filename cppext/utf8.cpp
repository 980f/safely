#include "utf8.h"
#include "bitwise.h" //bitfields

/** first byte tells you how many follow, number of leadings ones -2 (FE and FF are both 5)
 *  subsequent bytes start with 0b10xx xxxx 80..BF, which are not legal single byte chars.
 */

bool UTF8::numAlpha() const {
  return isalnum(raw) || isPresent("+-.", raw);
}

bool UTF8::startsName() const {
  return isalpha(raw);
}

bool UTF8::isWhite() const {
  return isspace(raw);
}

unsigned UTF8::numFollowers() const {
  if(u8(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here, C0 and C1  are not specfied so lump them in as well
    return 0;   //7 bits    128
  }
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
} // UTF8::numFollowers

unsigned UTF8::numFollowers(u32 unichar){
  if(unichar < (1U << 7)) {
    return 0;
  }
  if(unichar<(1U << (6 + 5))) {
    return 1;
  }
  if(unichar<(1U << (6 + 6 + 4))) {
    return 2;
  }
  if(unichar<(1U << (6 + 6 + 6 + 3))) {
    return 3;
  }
  if(unichar<(1U << (6 + 6 + 6 + 6 + 2))) {
    return 4;
  }
  if(unichar<(1U << (6 + 6 + 6 + 6 + 6 + 1))) {
    return 5;
  }
  //it appears unicode is likely to stop at 2G characters
  return 0;//not yet implementing invalid extensions.
} // UTF8::numFollowers

u8 UTF8::firstByte(u32 unichar, unsigned followers){
  if(followers) {
    u8 prefix(0xFC);
    prefix <<= (5 - followers);//1->C0, 2->E0 3->F0 4->F8
    unichar >>= (6 * followers);
    return prefix |= unichar;
  } else {
    return static_cast<u8>(unichar);//only the one byte needed.
  }
}

u8 UTF8::nextByte(u32 unichar, unsigned followers){
  unsigned shift = 6 * followers;
  unichar >>= shift;
  unichar &= fieldMask(6);
  unichar |= (1 << 7);
  return static_cast<u8>(unichar);//# truncate to 8 bits.
}

bool UTF8::in(const char *tokens) const {
  return isPresent(tokens, raw);
}
