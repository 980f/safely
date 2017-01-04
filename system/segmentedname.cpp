#include "segmentedname.h"
#include "textpointer.h"

SegmentedName::SegmentedName(bool rooted) : rooted(rooted){
  //#nada
}

bool SegmentedName::empty() const {
  return elements.quantity()==0;//naive implementation, doesn't deal with trivial entities.
}

unsigned SegmentedName::numSeperators(bool includeRootedness) const {
  return elements.quantity()+((includeRootedness&&rooted)?1U:0U);
}

void SegmentedName::purify(){
  for(auto index(indexer());index.hasNext();){
    if(index.next().empty()){
      index.removeLastNext();
    }
  }
}


void SegmentedName::prefix(TextKey parent){
  elements.insert(new Cstr(parent),0);
}

void SegmentedName::suffix(TextKey child){
  elements.append(new Cstr(child));
}


void SegmentedName::prefix(const Cstr &parent){
  elements.insert(new Text(parent),0);
}

void SegmentedName::suffix(const Cstr &child){
  elements.append(new Text(child));
}

ChainScanner<Cstr> SegmentedName::indexer(){
  return ChainScanner<Cstr>(elements);
}

ConstChainScanner<Cstr> SegmentedName::indexer() const {
  return ConstChainScanner<Cstr>(elements);
}
