#include "pathparser.h"
#include "malloc.h"

#include "charformatter.h" //might be able to reduce to buffer.h

#include "numberformatter.h"


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
  return Text(path);
} // PathParser::pack

bool PathParser::parseInto(SegmentedName &pieces, Text &packed, char seperator){
  CharScanner scan( Cstr::violate(packed.c_str()),packed.length());
  bool rooted(false);

  while(scan.hasNext()) {
    if(scan.next()==seperator) {
      scan.previous() = 0;//null terminator for Text constructor
      if(scan.ordinal()==1) {//1st char was an(other) seperator
        if(pieces.quantity()==0) {
          rooted = true;
        } else {          //todo: what do we do with empty path elements?
          // merge multiple successive seperators into effectively just one.
          scan.grab(scan);
          continue;
        }
      }
      pieces.append(new Text(scan.internalBuffer()));
      scan.grab(scan);
    }
  }
  return rooted;
} // PathParser::parseInto

Text PathParser::makeNumber(double value){
  NumberFormatter formatter(17);
  return formatter(value,false);
}
