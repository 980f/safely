#include "pathparser.h"
#include "malloc.h"

#include "charformatter.h"

#include <halfopen.h>

using namespace PathParser;

Text PathParser::pack(const SegmentedName &pieces, const Rules &rule, Converter &&converter){
  unsigned quantity=pieces.quantity();
  if(quantity==0){
    return Text();
  }

  unsigned bytesNeeded =quantity-1+rule.before+rule.after;//number of seperators

  for(auto index(pieces.indexer());index.hasNext();){
    bytesNeeded += converter.length(index.next());
  }

  char *path(static_cast<char *>( malloc(Zguard(bytesNeeded))));//@DEL when returned Text object is deleted
  path[bytesNeeded]=0;//null terminate since we didn't pre-emptively calloc.
  CharFormatter packer(path,bytesNeeded);
  for(auto feeder(pieces.indexer());feeder.hasNext();) {
    if(feeder.ordinal()>0 || rule.before){//if not first or if put before first
      packer.next() = rule.slash;
    }
    converter(feeder.next(),packer);
  }
  if(rule.after && packer.used()>0) {//only append trailing slash if there is something ahead of it
    packer.next() = rule.slash;
  }
  //and in case we overestimated the length needed:
  packer.printChar(0);//null terminate since we didn't pre-emptively calloc.

  return Text(path);//when you destroy the Text the data malloc'd above is freed
}


Rules PathParser::parseInto(SegmentedName &pieces, const Text &packed, char seperator){
  Rules bracket(seperator,false,false);
  Indexer<const char> scan( packed.c_str(),packed.length());

  Text::Chunker cutter(packed);
  if(scan.hasNext()&&scan.peek()==seperator){//if begins with seperator
    bracket.before=true;
    scan.next();
    if(!scan.hasNext()){//solitary slash was reporting both before and after slashing
      return bracket;//exiting early here gives us pre, not post, no pieces.
    }
  }
  cutter.lowest=scan.ordinal();//
  //cutter.highest is still invalid

  while(scan.hasNext()) {
    if(scan.next()==seperator) {
      cutter.highest=scan.ordinal()-1;
      if(cutter.empty()){
        //adjacent seperators are as if they are one
        cutter.lowest+=1;//which has a side effect of making it invalid, which is good
      } else {
        pieces.suffix(cutter(1/*next chunk starts past the seperator*/));
      }
    }
  }
  //the following is suspect
  if(scan.contains(cutter.lowest)){
    cutter.highest=scan.ordinal();
    if(cutter.empty()){
      bracket.after=true;
    } else {
      pieces.suffix(cutter(0));
    }
  } else {
    //if (cutter.lowest==span.length), but not if empty string
    bracket.after=scan.ordinal()>0;
  }
  return bracket;
} // PathParser::parseInto

Rules::Rules(char slash, bool after, bool before):slash(slash),after(after),before(before){
  //#nada
}

