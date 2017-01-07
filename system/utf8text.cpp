#include "utf8text.h"
#include "utf8.h"
#include "buffer.h"

using namespace Utf8Text;

/** */
enum Event {
  End=0, //in case we get confused and think it is a null char
  More,
  Done,
  Plain,
  Xand, //only from decoder
  First //only from Escaper
};


/** utf8 to u32 push extractor */
class Utf8Escaper {
public:
  CountDown followers = 0;
  Unichar uch = 0;
  bool bigu;

  Event operator ()( UTF8 ch){
    if(ch.is(0)){
      if(followers){//was in middle of sequence
        UTF8::pad(uch, followers);//same plane, better than total garbage
      } else {
        uch=0;//safety
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
      return First;
    }
    uch=ch;
    return Plain;
  }

  /** @returns assembled char, clears local copy to expose bugs */
  Unichar fetch(){
    ClearOnExit<Unichar> coe(uch);
    return uch;
  }

};

//utf8 -> u32 -> \u or \U
Text Utf8Text::encode(Cstr utf8s, Index presized){
  if(! presized.isValid()){
    presized=encodedLength(utf8s);
    if(! presized.isValid()){
      return Text();
    }
  }
  Text retval(Zguard(presized));
  Indexer<char> packer(retval.violated(),presized);
  const char *peeker=utf8s.c_str();


  Utf8Escaper ex;
  while(packer.hasNext()){
    switch (ex(*peeker++)) {
    case Event::Xand:
      break;//won't happen
    case Event::First:
    case Event::More:
      break;
    case Event::Done:{
      int hexits=ex.bigu?8:4;
      Unichar uch=ex.fetch();
      if(packer.stillHas(hexits+2)){
        packer.next()='\\';
        packer.next()=ex.bigu?'U':'u';
        while(hexits-->0){
          packer.next()=UTF8::hexNibble(uch,hexits);
        }
      }
    } break;
    case Event::Plain:
//todo:00 generate slash escapes
      packer.next()=ex.fetch();
      break;
    case Event::End:
      //if uch is not zero then we clipped the last char
      return retval;//abnormal exit, smile and go on.
    }
  }
  return retval;
}

Index Utf8Text::encodedLength(Cstr utf8s){
  if(utf8s.empty()) {
    return 0;
  }
  int totes(0);
  const char *scan = utf8s;
  Utf8Escaper ex;
  while(true) {
    switch (ex(*scan++)) {
    case Event::First:
      break;
    case Event::More:
      break;
    case Event::Done:
      totes += ex.bigu?10:6;
      break;
    case Xand://todo: how many passed through?
      totes += ex.bigu?10:6;
      --scan;
      break;
    case Event::Plain:
      //todo: slash escapes
      ++totes;
      break;
    case Event::End:
      if(ex.uch) {//we ended with a partial utf8 char
        totes += 10;//worst case COA
      }
      return totes;
    }
  }
  return BadIndex;//bug
} // Utf8Text::encodedLength


/** push parser to convert expanded codes back to utf8 */
class Utf8Decoder {
public:
  bool slashing = false;
  bool xing = false; //\x.....
  CountDown  uchers = 0;
  CountDown octers = 0; //\nnn

  Unichar uch = 0;
  bool bigu;

  Event operator ()( UTF8 ch){
    if(ch.is(0)){//end of data
      if(uchers){//partial \u series
        uch<<=(4*uchers);
      } else if(octers){//partial \octal
        uch<<=(3*octers);
      } else if(xing){//\x at end of string, perfectly legal and somewhat common \x00
        //uch copacetic
      } else if(slashing){
        uch='\\';  //trailing slash is a slash
      } else {
        uch=0;
      }
      return End;
    }

    if(uchers--) {
      uch <<= 4;
      uch |= ch.hexDigit();
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
        uch <<= 4;
        uch |= ch.hexDigit();
        return More;
      }
      if(ch.is('\\')) {
        slashing = true;
        return Plain;//take uch but don't resent the last char
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
      if(ch.in("abfnrtv\\'\"?")) {//these compress when de-escaping c slashes
        uch= ch;//todo:00 look up thing
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

  /** @returns assembled char, clears local copy to expose bugs */
  Unichar fetch(){
    ClearOnExit<Unichar> coe(uch);
    return uch;
  }

};



Index Utf8Text::decodedLength(Cstr utf8s,bool ctoo){
  if(utf8s.empty()) {
    return 0;
  }
  int totes(0);
  const char *scan = utf8s;
  Utf8Decoder dx;
  //#not using elseif below because of one exception and to make it easy to reorder the tests.
  while(true) {
    switch(dx(*scan++)){
    case First://doesn't happen
      break;//aavert warning
    case More://accumulating more info
      break;
    case Xand://need to resent last char
      --scan;
      //JOIN
    case Done: //uch is utf8 char
      totes += UTF8::numFollowers(dx.uch) + 1;
      break;
    case Plain:
      //todo: c-escapes here
      totes += 1;
      break;
    case End:
      if(dx.uch){//partial last char ...
        totes += UTF8::numFollowers(dx.uch) + 1;
      }
      return totes;
    }
  }
  return BadIndex;
} // Utf8Text::encodedLength

Text Utf8Text::decode(Cstr utf8s, Index presized){
  if(! presized.isValid()){
    presized=decodedLength(utf8s,false);
    if(! presized.isValid()){
      return Text();
    }
  }
  Text retval(Zguard(presized));
  Indexer<char> packer(retval.violated(),presized);
  const char *peeker=utf8s.c_str();

  Utf8Decoder dx;
  //#not using elseif below because of one exception and to make it easy to reorder the tests.
  while(packer.hasNext()) {
    switch(dx(*peeker++)){
    case First://doesn't happen
      break;//avert warning
    case More://accumulating more info
      break;
    case Xand://need to resent last char
      --peeker;
      //JOIN
    case Done:{ //uch is utf8 char
      unsigned nf=UTF8::numFollowers(dx.uch);
      if(packer.stillHas(nf+ 1)){
        Unichar uch=dx.fetch();
        packer.next()=UTF8::firstByte(uch,nf);
        while(nf-->0){
          packer.next()=UTF8::nextByte(uch,nf);
        }
      } else {
        //unexpectedly out of room
      }
    }
      break;
    case Plain:
      //todo: c-escapes here
      packer.next()=dx.fetch();
      break;
    case End:
      if(dx.uch){//partial last char ...
        //what do we do?
      }
      return retval;
    }
  }
  return retval;
}
