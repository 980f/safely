#include "utf8transcoder.h"
#include "localonexit.h"

/** In the string below there are pairs of (escape code: escaped char) e.g. n\n
 * it looks confusing as some of the chars being escaped are unchanged by the process.
 * when removing a slash you search for the char and pass back the odd member of the pair
 * when adding slashes you find the char and emit a slash and the even member of the pair
*/
static const char *SeaScapes="a\a" "b\b" "f\f" "n\n" "r\r" "t\t" "v\v" "\\\\" "//" "''" "??" "\"\"";


bool needsSlash(char raw){
  return Index(Cstr(SeaScapes).index(raw)).isOdd();//isOdd includes checking for valid.
}

/** while named for one usage this works symmetrically */
char slashee(char afterslash){
  Index present= Cstr(SeaScapes).index(afterslash);
  if(present.isValid()){
    return SeaScapes[1^present];//xor with 1 swaps even and odd.
  } else {
    return afterslash;
  }
}


Unichar Unicoder::fetch(){
  ClearOnExit<Unichar> coe(uch);
  cleanup();
  return uch;
}


Utf8Escaper::Event Utf8Escaper::operator ()(UTF8 ch){
  if(ch.is(0)){
    if(cleanup()){//was not in middle of sequence
      uch=0;//safety, probably already 0
    }
    return End;
  }

  if(followers--) {
    ch.moreBits(uch);
    if(followers.done()) {
      //figure \u and 4 or \U and 8
      bigu=uch >= (1 << 16);
      return Done;
    } else {
      return More;
    }
  }

  bigu=false;//only valid after Done until next push
  if(ch.isMultibyte()) {
    //we need nf, but so does firstBits (to save from having to call it twice).
    ch.firstBits(uch,followers = ch.numFollowers());
    return More;//distinguishing the first was hassle, not benefit.
  }
  uch=ch;
  return Plain;
}

bool Utf8Escaper::cleanup(){
  if(followers){//was in middle of sequence
    UTF8::pad(uch, followers);//same plane, better than total garbage
    followers=0;
    return false;
  }
  return true;
}

bool Utf8Decoder::cleanup(){
  if(uchers){//partial \u series
    uch<<=(4*uchers);
    uchers=0;
    return false;
  } else if(octers){//partial \octal
    uch<<=(3*octers);
    octers=0;
    return false;
  } else if(xing){//\x at end of string, perfectly legal and somewhat common \x00
    //uch copacetic
    xing=0;
    return false;
  } else if(slashing){
    uch='\\';  //trailing slash is a slash
    slashing=0;
    return false;
  }
  return true;
}

Utf8Decoder::Event Utf8Decoder::operator ()(UTF8 ch){
  if(ch.is(0)){//end of data
    if(cleanup()){
      uch=0;//probably already was, just protecting against ill advised future changes to the code.
    }
    return End;
  }

  if(uchers--) {
    ch.hexDigit(uch);
    if(uchers.done()) {
      return Done;
    }
    return More;//feed me Seymour ;)
  }

  if(octers--) {
    uch <<= 3;
    uch |= ch.raw&7;
    if(octers.done()){
      return Done;
    }
    return More;
  }

  if(xing) {
    if(ch.isHexDigit()) {
      ch.hexDigit(uch);
      return More;
    }
    xing=false;
    if(ch.is('\\')) {
      slashing = true;
      return Done;//take uch but don't resent the last char
    }
    //user gets two: uch and he'll have to lookback and repush the last item pushed.
    return Xand;
  }

  if(flagged(slashing)) {
    if(ch.is('u')) {
      uch = 0;
      uchers = 4;
      return More;
    }
    if(ch.is('U')) {
      uch = 0;
      uchers = 8;
      return More;
    }
    if(ch.isDigit()) {
      uch=0;
      octers = 3;
      return More;
    }

    if(ch.is('x')) {
      uch = 0;
      xing = true;
      return More;
    }
    if(ch.in(SeaScapes)) {//these compress when de-escaping c slashes
      uch= slashee(ch);//slash follower to not slashed?
      return Plain;
    }
  }
  ////// this must follow all the escaped tests
  if(ch.is('\\')) {
    slashing = true;
    return More;
  }
  uch=ch;
  return Plain;
}

