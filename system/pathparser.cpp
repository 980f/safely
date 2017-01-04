#include "pathparser.h"
#include "malloc.h"

#include "charformatter.h" //might be able to reduce to buffer.h

#include "numberformatter.h"

#include <halfopen.h>


Text PathParser::pack(const SegmentedName &pieces, char seperator){
  unsigned bytesNeeded = Zguard(pieces.mallocLength(1));
  char *path(static_cast<char *>( malloc(bytesNeeded)));
  CharFormatter packer(path,bytesNeeded);
  auto feeder(pieces.indexer());
  if(pieces.rooted && feeder.hasNext()) {
    packer.next() = seperator;
  }
  while (feeder.hasNext()) {
    packer.next() = seperator;
    packer.printString(feeder.next());
  }
  return Text(path);//when you destroy the Text the data malloc'd above is freed
} // PathParser::pack




bool PathParser::parseInto(SegmentedName &pieces, Text &packed, char seperator){
  CharScanner scan( Cstr::violate(packed.c_str()),packed.length());
  bool rooted(scan.peek()==seperator);

  Span cutter(scan.ordinal(),BadIndex);
  while(scan.hasNext()) {
    if(scan.next()==seperator) {
      cutter.highest=scan.ordinal();//halfopen interval is nice here.
      if(scan.ordinal()==1) {//1st char was an(other) seperator
        if(pieces.empty()) {
          rooted = true;
        }
      }
      pieces.suffix(*new Text(scan.internalBuffer()));
      scan.grab(scan);
    }
  }
  return rooted;
} // PathParser::parseInto

Text PathParser::makeNumber(double value){
  NumberFormatter formatter(17);
  return formatter(value,false);
}
