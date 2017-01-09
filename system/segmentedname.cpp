#include "segmentedname.h"
#include "textpointer.h"

#include "utf8text.h"
#include "urltext.h"


SegmentedName::SegmentedName() {
  //#nada
}

bool SegmentedName::empty() const {
  return quantity()==0;//naive implementation, doesn't deal with trivial entities.
}

void SegmentedName::purify(){
  for(auto index(indexer());index.hasNext();){
    if(index.next().empty()){
      index.removeLastNext();
    }
  }
}

void SegmentedName::copycat(const SegmentedName &other){
  for(auto index=other.indexer();index.hasNext();){
    auto item=index.next();
    this->suffix(Text(item));//construction here forces copy
  }
}

void SegmentedName::transfer(SegmentedName &other){
  for(auto index=other.indexer();index.hasNext();){
    auto item=index.next();
    this->suffix(Text(item,true));//construction here nulls original entity
  }
  other.clear();//they are all null so might as well ditch them.
}

void SegmentedName::prefix(TextKey parent){
  insert(new Cstr(parent),0);
}

void SegmentedName::suffix(TextKey child){
  append(new Cstr(child));
}


void SegmentedName::prefix(const Cstr &parent){
  insert(new Text(parent),0);
}

void SegmentedName::suffix(const Cstr &child){
  append(new Text(child));
}

ChainScanner<Cstr> SegmentedName::indexer(){
  return ChainScanner<Cstr>(*this);
}

ConstChainScanner<Cstr> SegmentedName::indexer() const {
  return ConstChainScanner<Cstr>(*this);
}
