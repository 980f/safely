#include "pathparser.h"
#include "malloc.h"

#include "charformatter.h"

#include <halfopen.h>

using namespace PathParser;

Text PathParser::pack(const SegmentedName &pieces, const Rules &rule){
  unsigned quantity=pieces.quantity();
  if(quantity==0){
    return Text();
  }
  unsigned numSeperators=quantity-1+rule.before+rule.after;
  unsigned bytesNeeded = Zguard(numSeperators+pieces.contentLength(false,false));
  char *path(static_cast<char *>( malloc(bytesNeeded)));//@DEL when returned Text object is deleted
  CharFormatter packer(path,bytesNeeded);
  auto feeder(pieces.indexer());
  while (feeder.hasNext()) {
    if(packer.ordinal()>0 || rule.before){//if not first or if put before first
      packer.next() = rule.slash;
    }
    packer.printString(feeder.next());
  }
  if(rule.after && packer.used()>0) {//only append trailing slash if there is something ahead of it
    packer.next() = rule.slash;
  }
  packer.printChar(0);//null terminate since we didn't pre-emptively calloc.

  return Text(path);//when you destroy the Text the data malloc'd above is freed
}

//Text PathParser::pack(const SegmentedName &pieces, char seperator){
//  return PathParser::pack(pieces,seperator,Rules());
//}

Rules PathParser::parseInto(SegmentedName &pieces, const Text &packed, char seperator){
  Rules bracket(seperator,false,false);
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
  if(scan.contains(cutter.lowest)){
    cutter.highest=scan.ordinal();//halfopen interval is nice here.
    if(cutter.empty()){
      bracket.after=true;
    } else {
      pieces.suffix(*new Text(scan.internalBuffer(),cutter));
    }
  }
  return bracket;
} // PathParser::parseInto

Rules::Rules(char slash, bool after, bool before):slash(slash),after(after),before(before){
  //#nada
}
