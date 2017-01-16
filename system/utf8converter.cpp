#include "utf8converter.h"
#include "utf8transcoder.h"



unsigned Utf8ConverterOut::length(const char *source) const{
  if(Cstr(source).empty()) {
    return 0;
  }
  int totes(0);
  const char *scan = source;
  Utf8Escaper ex;
  while(true) {
    switch (ex(*scan++)) {
    case Utf8Escaper::More:
      break;
    case Utf8Escaper::Done:
      totes += ex.bigu?10:6;
      break;
    case Utf8Escaper::Plain:
      if(needsSlash(ex.uch)){
        ++totes;//for the slash
      }
      ++totes;
      break;
    case Utf8Escaper::End:
      if(ex.uch) {//we ended with a partial utf8 char
        totes += 10;//worst case COA
      }
      return totes;
    }
  }
  return BadIndex;//#bug if you get here
}


void Utf8ConverterOut::operator()(const char *peeker, Indexer<char> &packer){
  Utf8Escaper ex;
  while(packer.hasNext()){
    switch (ex(*peeker++)) {
    case Utf8Escaper::More:
      break;
    case Utf8Escaper::Done:{
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
    case Utf8Escaper::Plain:
//todo:00 generate slash escapes
      packer.next()=u8(ex.fetch());
      break;
    case Utf8Escaper::End:
      //todo: if uch is not zero then we clipped the last char
      return;
    }
  }
}

unsigned Utf8ConverterIn::length(const char *source) const {
  if(Cstr(source).empty()) {
    return 0;
  }
  int totes(0);

  Utf8Decoder dx;
  //#not using elseif below because of one exception and to make it easy to reorder the tests.
  while(true) {
    switch(dx(*source++)){
    case Utf8Decoder::More://accumulating more info
      break;
    case Utf8Decoder::Xand://need to resend last char
      --source;//resends
      //JOIN
    case Utf8Decoder::Done: //uch is utf8 char
      totes += UTF8::numFollowers(dx.fetch()) + 1;
      break;
    case Utf8Decoder::Plain:
      //todo: c-escapes here
      totes += 1;
      break;
    case Utf8Decoder::End:
      if(dx.uch){//partial last char ...
        totes += UTF8::numFollowers(dx.fetch()) + 1;
      }
      return totes;
    }
  }
  return BadIndex;
}

void Utf8ConverterIn::operator()(const char *peeker, Indexer<char> &packer){
  Utf8Decoder dx;
  //#not using elseif below because of one exception and to make it easy to reorder the tests.
  while(packer.hasNext()) {
    switch(dx(*peeker++)){
    case Utf8Decoder::More://accumulating more info
      break;
    case Utf8Decoder::Xand://need to resend last char
      --peeker;//resends last
      //JOIN
    case Utf8Decoder::Done:{ //uch is utf8 char
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
    case Utf8Decoder::Plain:
      //todo: c-escapes here
      packer.next()=u8(dx.fetch());
      break;
    case Utf8Decoder::End:
      if(dx.uch){//partial last char ...
        //what do we do?
      }
      return;
    }
  }
  return;
}

//todo: force 'remaining' constructor
UnicharScanner::UnicharScanner(const Indexer<const char> &utf8, int rewind):utf8(utf8,rewind){
  //#nada
}

bool UnicharScanner::hasNext() const {
  return utf8.hasNext();
}

Unichar UnicharScanner::next(){
  while(utf8.hasNext()){
    if ((ex(utf8.next())!=Utf8Escaper::More)){
      return ex.fetch();
    }
  }
  return 0;
}


bool UnicharReader::hasNext() const {
  return utf8.hasNext();
}

Unichar UnicharReader::next(){
  while(utf8.hasNext()){
    if ((ex(utf8.next())!=Utf8Escaper::More)){
      return ex.fetch();
    }
  }
  return 0;
}
