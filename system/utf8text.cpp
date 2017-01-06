#include "utf8text.h"
#include "utf8.h"

Utf8Text::Utf8Text(){

}

unsigned Utf8Text::encodedLength(Cstr utf8s){
  if(utf8s.empty()) {
    return 0;
  }
  int totes(0);
  //unicode assembler
  unsigned followers = 0;
  Unichar uch = 0;

  const char *scan = utf8s;
  while(UTF8 ch = *scan++) {
    if(followers) {
      ch.moreBits(uch);
      if(--followers==0) {
        //figure \u and 4 or \U and 8
        if(uch< (1 << 16)) {
          totes += 6;
        } else {
          totes += 10;
        }
      }
    } else if(ch.isMultibyte()) {
      followers = ch.numFollowers();
      ch.firstBits(uch);
    } else {
      ++totes;
    }
  }
  if(followers) {
    UTF8::pad(uch, followers);
    //we ended with a partial utf8 char
    totes += 10;//encoder will feed 0's so that at least we are in a close unicode plane.
  }
  return totes;
} // Utf8Text::encodedLength

unsigned Utf8Text::decodedLength(Cstr utf8s,bool ctoo){
  if(utf8s.empty()) {
    return 0;
  }
  int totes(0);
  bool slashing = false;
  bool xing = false; //\x.....
  unsigned uchers = 0;
  unsigned octers = 0; //\nnn

  Unichar uch = 0;
  const char *scan = utf8s;
  //#not using elseif below because of one exception and to make it easy to reorder the tests.
  while(UTF8 ch = *scan++) {

    if(uchers) {
      uch <<= 4;
      uch |= ch.hexDigit();
      if(--uchers==0) {
        totes += UTF8::numFollowers(uch) + 1;
      }
      continue;
    }

    if(octers) {
      --octers;
      //just need to ignore 3 digits. Not our problem if input is defective.
      continue;
    }

    if(flagged(slashing)) {
      if(ch.is('u')) {
        uch = 0;
        uchers = 4;
      } else if(ch.is('U')) {
        uch = 0;
        uchers = 8;
      } else if(ch.in("abfnrtv\\'\"?")) {//these compress when de-escaping c slashes
        totes += ctoo ? 1 : 2;
      } else if(ch.isDigit()) {
        octers = 3;
        totes += 1;//the slash and the next 3 will make a single char
      } else if(ch.is('x')) {
        uch = 0;
        xing = true;
      } else {//unrecognized escapes pass unmodified
        totes += 2;//gotta count that slash as well
      }
      continue;
    }

    if(xing) {
      if(ch.isHexDigit()) {
        uch <<= 4;
        uch |= ch.hexDigit();
        continue;
      } else {
        //uch is one or more utf8 chars
        totes += UTF8::numFollowers(uch) + 1;
        //and this non-hex could be a start of something else, so no 'continue'
      }
    }
    ////// this must follow all the escaped tests
    if(ch.is('\\')) {
      slashing = true;
      continue;
    }
    //char is nothing special
    ++totes;
  }
  return totes;
} // Utf8Text::encodedLength
