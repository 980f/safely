#include "pathparser.h"
#include "malloc.h"

#include "charformatter.h"

#include <halfopen.h>


Text PathParser::pack(const SegmentedName &pieces, char seperator, Brackets bracket){
  unsigned quantity=pieces.quantity();
  if(quantity==0){
    return Text();
  }
  unsigned numSeperators=quantity-1+bracket.before+bracket.after;
  unsigned bytesNeeded = Zguard(numSeperators+pieces.contentLength(false,false));
  char *path(static_cast<char *>( malloc(bytesNeeded)));//@DEL when returned Text object is deleted
  CharFormatter packer(path,bytesNeeded);
  auto feeder(pieces.indexer());
  while (feeder.hasNext()) {
    if(packer.ordinal()>0 || bracket.before){//if not first or if put before first
      packer.next() = seperator;
    }
    packer.printString(feeder.next());
  }
  if(bracket.after && packer.used()>0) {//only append trailing slash if there is something ahead of it
    packer.next() = seperator;
  }
  packer.printChar(0);//null terminate since we didn't pre-emptively calloc.

  return Text(path);//when you destroy the Text the data malloc'd above is freed
}

Text PathParser::pack(const SegmentedName &pieces, char seperator, bool after, bool before){
  return pack(pieces,seperator,PathParser::Brackets(after,before));
} // PathParser::pack


PathParser::Brackets PathParser::parseInto(SegmentedName &pieces, Text &packed, char seperator){
  PathParser::Brackets bracket;
  Indexer<const char> scan( packed.c_str(),packed.length());

  Span cutter;
  if(scan.hasNext()&&scan.peek()==seperator){//if begins with seperator
    bracket.before=true;
    scan.next();
    cutter.lowest=scan.ordinal();//
  } else {
    cutter.lowest=0;
  }//cutter.highest is still invalid

  while(scan.hasNext()) {
    if(scan.next()==seperator) {
      cutter.highest=scan.ordinal();//halfopen interval is nice here.
      if(cutter.empty()){
        //adjacent seperators are as if they are one
        cutter.lowest+=1;
      } else {
        pieces.suffix(*new Text(scan.internalBuffer(),cutter));
        cutter.leapfrog(1);
      }
    }
  }
  //the following is suspect
  if(cutter.lowest<scan.ordinal()){
    cutter.highest=scan.ordinal();//halfopen interval is nice here.
    if(cutter.empty()){
      bracket.after=true;
    } else {
      pieces.suffix(*new Text(scan.internalBuffer(),cutter));
    }
  }
  return bracket;
} // PathParser::parseInto

PathParser::Brackets::Brackets(bool after, bool before):after(after),before(before){
  //#nada
}
