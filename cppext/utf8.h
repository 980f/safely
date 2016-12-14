#ifndef UTF8_H
#define UTF8_H

#include "eztypes.h"
#include "cheapTricks.h" //isPresent
#include "ctype.h"

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

  bool operator ==(int ch) const {
    return raw == char(ch);
  }

  bool operator ==(UTF8 other) const {
    return raw == other.raw;
  }

//  operator int(void){  //so that we can switch(utf8) and also use traditional C string functionality.
//    return raw;
//  }

//  operator char(void){ //cuts down on compiler complaints, except in switches.
//    return raw;
//  }

  operator char(void)const{ //cuts down on compiler complaints, except in switches.
    return raw;
  }

//  operator bool(void){ //for while(*pointer) usage.
//    return raw != 0;
//  }

  /** numeric constant or enum name */
  bool numAlpha() const;

  bool isWhite() const {
    return isPresent(" \t\r\n", raw);
  }

  /** if you skip a byte then numFollowers will be illegal*/
  bool isMultibyte() const {
    return raw & 0x80; //treating illegals as multibyte.
  }

  /** only valid if first char of a sequence */
  int numFollowers(void) const;

  /** @returns number of 10xxxxxx bytes needed for given @param unichar unicode char*/
  static int numFollowers(u32 unichar);

  /** @returns 1st byte of sequence @given @param followers value returned from @see numFollowers(u32)*/
  static u8 firstByte(u32 unichar,int followers);

  /** @returns intermediate or final byte, @param followers is 0 for the final one */
  static u8 nextByte(u32 unichar,int followers);

  bool in(const char *tokens) const;

};

#endif // UTF8_H
