#ifndef UTF8_H
#define UTF8_H

#include "eztypes.h"  //~stdint
//#include "ignoresignwarnings.h"  //much type mangling is done herein, so you don't have to in your code :)

/** an int that can hold a UTF32 character
 * replace Glib gunichar usages */
typedef u32 Unichar;

#include "cheaptricks.h" //isPresent

#include "char.h" //makes a character like an object

/** represents one byte of a UTF8 multibyte character, not to be confused with a Unicode character which is a 32 bit entity
 * in addition to utf8 info this wraps ctype functions making them members of a char.
*/
class UTF8: public Char {
public:
  UTF8(char raw = 0):Char(raw){
  }

  UTF8&operator =(char raw){
    this->raw = raw;
    return *this;
  }

  bool is(UTF8 other) const  noexcept{
    return is(other.raw);
  }

  /** compare byte.
   * @returns whether this is the same value as @param ch. */
  bool is(int ch) const noexcept {
    return raw == char(ch);
  }


  /** if you skip a byte then numFollowers will be illegal*/
  bool isMultibyte() const noexcept {
    return raw & 0x80; //treating illegals as multibyte.
  }

  /** only valid if first char of a UTF8 sequence */
  unsigned numFollowers(void) const noexcept;

  /** bits extracted from this byte, @param nf is value from numFollers, ~0 means call numFollowers else if already done pass tha back in.*/
  void firstBits(Unichar &uch, unsigned nf=~0) const noexcept;
  /** merges bits from tihs presumed to be continuation byte into @param uch */
  void moreBits(Unichar &uch) const noexcept;
  /** pretend remaining bytes were all zeroes */
  static void pad(Unichar &uch, unsigned followers) noexcept;

  /** @returns number of 10xxxxxx bytes needed for given @param unichar unicode char.*/
  static unsigned numFollowers(u32 unichar)noexcept;

  /** @returns 1st byte of sequence given @param followers value returned from @see numFollowers(u32)*/
  static u8 firstByte(u32 unichar,unsigned followers)noexcept;

  /** @returns intermediate or final byte, @param followers is 0 for the final one */
  static u8 nextByte(u32 unichar,unsigned followers) noexcept;

  static char hexNibble(Unichar uch,unsigned sb) noexcept;


}; // class UTF8

#if 0

to stream a Unichar as utf8:

int followers = numFollowers(unichar);
out << firstByte(unichar,followers);
while(followers-->0) {
  out << nextByte(unichar,followers);
}

coalescing utf8 stream into a unichar:

int numfollowers=utf8.numFollowers();
if(numfollowers>0){
  Unichar uch=0;
  utf8.firstBits(uch);
  while(numfollowers-->0){
    utf8=next();
    utf8.moreBits(uch);
  }
}

#endif

#endif // UTF8_H
