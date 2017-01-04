#ifndef UTF8_H
#define UTF8_H


#include "eztypes.h"  //~stdint

/** an int that can hold a UTF32 character
 * replace Glib gunichar usages */
typedef u32 Unichar;

#include "cheaptricks.h" //isPresent
#include "ctype.h"

/** represents one byte of a UTF8 multibyte character, not to be confused with a Unicode character which is a 32 bit entity
 * in addition to utf8 info this wraps ctype functions making them members of a char.

*/
class UTF8 {
public:
  char raw;
  UTF8(char raw = 0){
    this->raw = raw;
  }

  UTF8&operator =(char raw){
    this->raw = raw;
    return *this;
  }

  /** compare byte.
   * @returns whether this is the same value as @param ch. */
  bool is(int ch) const {
    return raw == char(ch);
  }

  /** @returns @see is() */
  bool operator ==(int ch) const {
    return is(ch);
  }

  bool is(UTF8 other) const {
    return is(other.raw);
  }

//  operator int(void){  //so that we can switch(utf8) and also use traditional C string functionality.
//    return raw;
//  }

//  operator char(void){ //cuts down on compiler complaints, except in switches.
//    return raw;
//  }

  operator char(void) const { //cuts down on compiler complaints, except in switches.
    return raw;
  }

//  operator bool(void){ //for while(*pointer) usage.
//    return raw != 0;
//  }

  /** @returns whether this is allowed in numeric constant or enum name */
  bool numAlpha() const;

  /** @returns whether this is first char of an identifier, per JSON and C++ rules. This is pretty much anything that isn't a number, punctuation or a control char */
  bool startsName() const;

  /** @returns whether this is first char of an number image, per JSON and C++ rules */
  bool startsNumber() const;

  /** @returns whether this is non-initial char of a number. Doesn't retain state so defective numbers will still get a true here*/
  bool isInNumber() const;

  /** @returns whether this is considered whitespace */
  bool isWhite() const;

  bool in(const char *tokens) const;

  /** if you skip a byte then numFollowers will be illegal*/
  bool isMultibyte() const {
    return raw & 0x80; //treating illegals as multibyte.
  }

  /** only valid if first char of a UTF8 sequence */
  unsigned numFollowers(void) const;

  /** bits extracted from this byte*/
  void firstBits(Unichar &uch) const;
  /** merges bits from tihs presumed to be continuation byte into @param uch */
  void moreBits(Unichar &uch) const;
  /** pretend remaining bytes were all zeroes */
  static void pad(Unichar &uch, unsigned followers);

  /** @returns number of 10xxxxxx bytes needed for given @param unichar unicode char.*/
  static unsigned numFollowers(u32 unichar);

  /** @returns 1st byte of sequence given @param followers value returned from @see numFollowers(u32)*/
  static u8 firstByte(u32 unichar,unsigned followers);

  /** @returns intermediate or final byte, @param followers is 0 for the final one */
  static u8 nextByte(u32 unichar,unsigned followers);

}; // class UTF8

#if 0

to stream a Unichar:

int followers = numFollowers(unichar);
out << firstByte(unichar,followers);
while(followers-->0) {
  out << nextByte(unichar,followers);
}

#endif

#endif // UTF8_H
