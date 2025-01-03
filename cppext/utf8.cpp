#include "utf8.h"
#include "bitwise.h" //bitfields

// #include "ignoresignwarnings.h"

/** UTF8 encoding:
 * first byte tells you how many follow, number of leading ones -2 (FE and FF are both 5)
 *  subsequent bytes start with 0b10xx xxxx 80..BF, which are not legal single byte chars.
 */
unsigned UTF8::numFollowers() const noexcept {
  if (uint8_t(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here, C0 and C1  are not specified so we lump them in as well
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
  if (uint8_t(raw) < 0xC0) { //80..BF are illegal raw, we ignore that here, C0 and C1  are not specified so we lump them in as well
    //we can't trust numFollowers if this byte isn't multiByte
    uch = 0; //illegal argument
  } else {
    if (nf == ~0) { //unknown, so go compute
      nf = numFollowers();
    }
    //need to keep 6-nf bits
    uch = fieldMask(6 - nf) & uint8_t(raw);
  }
}

void UTF8::moreBits(Unichar &uch) const noexcept {
  uch <<= 6;
  uch |= fieldMask(6) & uint8_t(raw);
}

void UTF8::pad(Unichar &uch, unsigned followers) noexcept {
  uch <<= (6 * followers);
}

unsigned UTF8::numFollowers(uint32_t unichar) noexcept {
  if (unichar < (1U << 7)) { //quick exit for ascii
    return 0;
  }
  for (int f = 1; f < 6; ++f) {
    unsigned bits = (6 * f) + (6 - f);
    if (unichar < (1U << bits)) {
      return f;
    }
  }
  //it appears Unicode is likely to stop at 2G characters
  return 0; //not yet implementing invalid extensions.
} // UTF8::numFollowers

uint8_t UTF8::firstByte(uint32_t unichar, unsigned followers) noexcept {
  if (followers) {
    uint8_t prefix(0xFC);
    prefix <<= (5 - followers); //1->C0, 2->E0 3->F0 4->F8
    unichar >>= (6 * followers);
    return prefix |= unichar;
  } else {
    return static_cast<uint8_t>(unichar); //only the one byte needed.
  }
}

uint8_t UTF8::nextByte(uint32_t unichar, unsigned followers) noexcept {
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
