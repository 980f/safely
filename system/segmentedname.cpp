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
  for(auto index=other.cindexer();index.hasNext();){
    const Text &item=index.next();
    this->suffix(item.c_str());//c_str() here forces copy, see transfer where we take the content
  }
}

void SegmentedName::transfer(SegmentedName &other){
  for(auto index=other.indexer();index.hasNext();){
    Text &item=index.next();
    this->suffix(item);//construction here nulls original entity
  }
  other.clear();//they are all null so might as well ditch them.
}

void SegmentedName::prefix(TextKey parent){
  prefix(Cstr(parent));
}

void SegmentedName::suffix(TextKey child){
  suffix(Cstr(child));
}

void SegmentedName::prefix(Cstr &&parent){
  insert(new Text(parent,false),0);
}

void SegmentedName::suffix(Cstr &&child){
  append(new Text(child,false));
}

void SegmentedName::prefix(Text &parent){
  insert(new Text(parent),0);
}

void SegmentedName::suffix(Text &child){
   append(new Text(child));
}

ChainScanner<Text> SegmentedName::indexer(){
  return ChainScanner<Text>(*this);
}

ConstChainScanner<Text> SegmentedName::cindexer() const {
  return ConstChainScanner<Text>(*this);
}
