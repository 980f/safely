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
  unsigned followers =0;
  Unichar uch=0;

  const char *scan = utf8s;
  while(UTF8 ch = *scan++) {
    if(followers){
      ch.moreBits(uch);
      if(--followers==0){
        //figure \u and 4 or \U and 8
        if(uch< (1<<16)){
          totes+=6;
        } else {
          totes+=10;
        }
      }
    } else if(ch.isMultibyte()){
      followers = ch.numFollowers();
      ch.firstBits(uch);
    } else {
      ++totes;
    }
  }
  if(followers){
    UTF8::pad(uch, followers);
    //we ended with a partial utf8 char
    totes+=10;//encoder will feed 0's so that at least we are in a close unicode plane.
  }
  return totes;
}

unsigned Utf8Text::decodedLength(Cstr utf8s,bool ctoo){
  if(utf8s.empty()) {
    return 0;
  }
  int totes(0);
  bool slashing=false;
  bool xing=false; //\x.....
  unsigned uchers =0;
  unsigned octers=0; //\nnn

  Unichar uch=0;
  const char *scan = utf8s;
  while(UTF8 ch = *scan++) {
    if(uchers){
      uch<<=4;
      uch|= ch.hexDigit();
      if(--uchers==0){
        totes+=UTF8::numFollowers(uch)+1;
      }
    } else if(flagged(slashing)){
      if(ch.is('u')){
        uchers=4;
      } else if(ch.is('U')){
        uchers=8;
      } else if(ch.in("abfnrtv\\'\"?")){//these compress when de-escaping c slashes
        totes+=ctoo?1:2;
      } else {//unrecognized escapes pass unmodified
        totes+=2;//gotta count that slash as well
      }
    } else if(ch.is('\\')){
      slashing=true;
    } else {//char is nothing special
      ++totes;
    }
  }
  return totes;
} // Utf8Text::encodedLength
