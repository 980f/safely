#include "utf8text.h"
#include "utf8.h"

Utf8Text::Utf8Text(){

}

int Utf8Text::encodedLength(Cstr utf8s){
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

int Utf8Text::decodedLength(Cstr utf8s){
  return -1;
} // Utf8Text::encodedLength
