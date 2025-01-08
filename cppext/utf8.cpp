#include "utf8.h"
#include "bitwise.h" //bitfields

// #include "ignoresignwarnings.h"

/** UTF8 encoding:
 * first byte tells you how many follow, number of leading ones -2 (FE and FF are both 5)
 *  subsequent bytes start with 0b10xx xxxx 80..BF, which are not legal single byte chars.
 */
unsigned UTF8::numFollowers() const noexcept {
  if (uint8_t(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here,
    return 0; //7 bits    128
  }
  //unrolled loop:
  if (uint8_t(raw) < 0xE0) {
    return 1; //5 bits,  6 bits 2k
  }
  if (uint8_t(raw) < 0xF0) {
    return 2; //4bits, 6, 6    64k
  }
  if (uint8_t(raw) < 0xF8) {
    return 3; //3 bits, 6,6,6  2M
  }
  if (uint8_t(raw) < 0xFC) { //not yet used
    return 4;
  }
  return 5; //not yet used, actually: FE and FF are the byte order marker of UTF16.
}

void UTF8::firstBits(Unichar &uch, unsigned nf) const noexcept {
  if (nf == ~0) { //unknown, so go compute
    nf = numFollowers();
  }
  //need to keep 6-nf bits
  uch = fieldMask(6 - nf) & uint8_t(raw);
}

void UTF8::moreBits(Unichar &uch) const noexcept {
  uch <<= 6;
  uch |= fieldMask(6) & uint8_t(raw);
}

void UTF8::pad(Unichar &uch, unsigned followers) noexcept {
  uch <<= (6 * followers);
}

unsigned UTF8::numFollowers(Unichar unichar) noexcept {
  if (0 == unichar >> 7) {
    return 0; //7 bits    128
  }
  if (0 == unichar >> 11) {
    return 1; //5 bits,  6 bits 2k
  }
  if (0 == unichar >> 16) {
    return 2;
  }
  if (0 == unichar >> 21) {
    return 3; //3 bits, 6,6,6  2M
  }
  //the next should not really happen, except for BOM which should not be passed to this guy in the first place
  if (0 == unichar >> 26) { //not yet used
    return 4;
  }
  return 5; //BOM, and user better check for this.
}

uint8_t UTF8::firstByte(Unichar unichar, unsigned followers) noexcept {
  if (followers) {
    uint8_t prefix(0xFC << (5 - followers)); //1->C0, 2->E0 3->F0 4->F8);
    return prefix | unichar >> (6 * followers);
  } else {
    return static_cast<uint8_t>(unichar); //only the one byte needed.
  }
}


uint8_t UTF8::nextByte(Unichar unichar, unsigned followers) noexcept {
  unsigned shift = 6 * followers;
  unichar >>= shift;
  unichar &= fieldMask(6);
  unichar |= (1 << 7);
  return static_cast<uint8_t>(unichar); //# truncate to 8 bits.
}

char UTF8::hexNibble(Unichar uch, unsigned sb) noexcept {
  uint8_t nib = 15 & (uch >> (sb * 4)); //push to low nib
  return nib > 9 ? 'A' + nib - 10 : '0' + nib;
}

bool UTF8::Decoder::start(uint8_t raw) {
  UTF8 utf8(raw);
  followers = utf8.numFollowers();
  if (followers > 0) {
    firstByte(unichar, followers);
    return true;
  }
  unichar = utf8.raw;
  errorcode = 0; //todo: detect 80..BF and report that here,  ditto BOM chars F8..FF.
  return false;
}

bool UTF8::Decoder::next(uint8_t raw) noexcept {
  //todo:M if 'ignore nulls' option is created this is where we check and return an unadjusted followers>0
  if (followers == 0) {
    errorcode = raw;
    return false;
  }
  UTF8 utf8(raw);
  if (MoreMarker == (raw & 0xC0)) { //proper 6 bit 'more' byte
    utf8.nextByte(unichar, followers);
    return followers-- > 0;
  }
  //missing trailing bytes if we get here
  pad(unichar, followers);
  errorcode = utf8.raw;
  return false;
}

uint8_t UTF8::Encoder::start(Unichar uch) {
  unichar = uch;
  followers = numFollowers(unichar);
  return firstByte(unichar, followers); //firstByte understands followers==0
}
