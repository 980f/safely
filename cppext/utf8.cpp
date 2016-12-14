#include "utf8.h"

//placeholder, to force build for library.


bool UTF8::numAlpha() const {
  return isalnum(raw) || isPresent("+-.", raw);
}

int UTF8::numFollowers() const {
  if(u8(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here, C0 and C1  are not specfied so lump them in as well
    return 0;
  }
  if(u8(raw) < 0xE0) {
    return 1;
  }
  if(u8(raw) < 0xF0) {
    return 2;
  }
  if(u8(raw) < 0xF8) {
    return 3;
  }
  if(u8(raw) < 0xFC) {
    return 4;
  }
  return 5;
}

int UTF8::numFollowers(u32 unichar){
  if(unichar < 0x80) {
    return 0;
  }
  int num(1);
  while(fieldMask(num * 6) < unichar) {
    ++num;
  }
  return num-1; //don't include leading byte
}

u8 UTF8::firstByte(u32 unichar, int followers){
  u8 acc(0xFC);//init for 5 followers
  acc<<=(5-followers);
  unichar >>= (6*followers);
  return acc |= unichar;
}

u8 UTF8::nextByte(u32 unichar, int followers){
  int shift = 6*followers;
  unichar >>= shift;
  unichar &= fieldMask(6);
  unichar|=0x80;
  return unichar;
}

bool UTF8::in(const char *tokens) const {
  return isPresent(tokens, raw);
}
