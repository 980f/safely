#pragma once

#include <cinttypes>
/** an int that can hold a UTF32 character */
using Unichar = uint32_t;

#include "char.h" //makes a character like an object

/** represents one byte of a UTF8 multibyte character, not to be confused with a Unicode character which is a 32 bit entity
 * in addition to utf8 info this wraps ctype functions making them members of a char.
 *
 * todo: add tests for BOM recognition, in which case caller needs to switch stream to a UTF16 class instead of complicating this one.
*/
class UTF8 : public Char {
public:
  static constexpr uint8_t MoreMarker = (1 << 7);
  //all 6's are the number of bits in a 'more' byte, a name wouldmake the code more obscure than it already is.
  UTF8(char raw = 0): Char(raw) {}

  UTF8 &operator =(char raw) {
    this->raw = raw;
    return *this;
  }

  bool is(UTF8 other) const noexcept {
    return is(other.raw);
  }

  /** compare byte.
   * @returns whether this is the same value as @param ch. */
  bool is(int ch) const noexcept {
    return raw == char(ch);
  }


  /** if you skip a byte then numFollowers will be illegal
   * You should probably call numFollowers and check value for zero rather than calling this function.
   */
  bool isMultibyte() const noexcept {
    return raw >> 7; //treating illegals as multibyte.
  }

  /** only valid if first char of a UTF8 sequence */
  unsigned numFollowers() const noexcept;

  /** bits extracted from this byte, @param nf is value from numFollers, ~0 means call numFollowers else if already done pass tha back in.*/
  void firstBits(Unichar &uch, unsigned nf = ~0) const noexcept;

  /** merges bits from this presumed to be continuation byte into @param uch */
  void moreBits(Unichar &uch) const noexcept;

  /** pretend remaining bytes were all zeroes */
  static void pad(Unichar &uch, unsigned followers) noexcept;

  /** @returns number of 10xxxxxx bytes needed for given @param unichar unicode char.*/
  static unsigned numFollowers(Unichar unichar) noexcept;

  /** @returns 1st byte of sequence given @param followers value returned from @see numFollowers(uint32_t)*/
  static uint8_t firstByte(Unichar unichar, unsigned followers) noexcept;

  /** @returns intermediate or final byte, @param followers is 0 for the final one */
  static uint8_t nextByte(Unichar unichar, unsigned followers) noexcept;

  static char hexNibble(Unichar uch, unsigned sb) noexcept;

  struct Decoder {
    Unichar unichar;
    unsigned followers;
    uint8_t errorcode;

    /**@returns whether more bytes are needed **/
    bool start(uint8_t raw);

    /**@returns whether more bytes are needed **/
    bool next(uint8_t raw) noexcept;
  };

  /** started with the ~32bit unicode point send out start byte and then while(....hasMore()) sendout(....next()*/
  struct Encoder {
    Unichar unichar;
    unsigned followers;

    /**@returns whether more bytes are needed **/
    uint8_t start(Unichar uch);

    bool hasMore() const {
      return followers > 0;
    }

    uint8_t next() noexcept {
      return nextByte(unichar, followers--);
    }
  };
}; // class UTF8
