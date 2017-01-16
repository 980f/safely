#include "pathparser.h"

#include <halfopen.h>

using namespace PathParser;

unsigned PathParser::length(ConstChainScanner<Cstr> indexer, const Rules &rule, Converter &&converter){
  if(!indexer.hasNext()){
    return 0;//don't inspect rules, i.e. rule.before only matters if we have something before
  }
  unsigned quantity=rule.after;//number of seperators

  while(indexer.hasNext()){
    quantity+= 1+converter.length(indexer.next());
  }
  if(!rule.before){
    --quantity;
  }
  return quantity;
}

void PathParser::packInto(Indexer<char> &packer,ConstChainScanner<Cstr>feeder,const Rules &rule, Converter &&converter){
  while(feeder.hasNext()) {
    if(feeder.ordinal()>0 || rule.before){//if not first or if put before first
      packer.next() = rule.slash;
    }
    converter(feeder.next(),packer);
  }
  if(rule.after && packer.used()>0) {//only append trailing slash if there is something ahead of it
    packer.next() = rule.slash;
  }
  //and in case we overestimated the length needed:
  packer.next()=0;//null terminate since we didn't pre-emptively calloc.
}

Text PathParser::pack(const SegmentedName &pieces, const Rules &rule, Converter &&converter){
  unsigned bytesNeeded=length(pieces.indexer(),rule,converter.forward());
  Indexer<char> packer=Indexer<char>::make(bytesNeeded,true);
  packInto(packer,pieces.indexer(),rule,converter.forward());
  return Text(packer.internalBuffer());//when you destroy the Text the data malloc'd above is freed
}

Rules PathParser::parseInto(SegmentedName &pieces, const Text &packed, char seperator){
  Rules bracket(seperator,false,false);
  Indexer<const char> scan( packed.c_str(),packed.length());

  Text::Chunker cutter(scan.internalBuffer());
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


