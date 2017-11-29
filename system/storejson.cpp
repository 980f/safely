//"(C) Andrew L. Heilveil, 2017"
#include "safely.h"
#include "storejson.h"
#include "textpointer.h"
#include "cheaptricks.h"


StoreJsonParser::StoreJsonParser(Indexer<char> &data):AbstractJSONparser(core),core(data){
  stats.reset();
  parser.reset(true);
}


StoreJsonConstructor::StoreJsonConstructor(Indexer< char> &data):data(data){//might want to getHead or Tail vs copy construct.
  //#nada
}

Text StoreJsonConstructor::extract(Span &span) {
  return Text(reinterpret_cast<const char*>(data.internalBuffer()),span);
}

Storable *StoreJsonConstructor::applyToChild(Storable *parent, Text &name, bool haveValue, Text &value, bool valueQuoted) {
  Storable *nova=nullptr;
  if(parent){
    if(name.empty()){
      nova=&parent->addChild("");//typically an array element, do NOT make all nameless entities the same entity.
    } else {
      nova=parent->findChild(name,true);
    }
  } else {
    root = &Storable::Groot(name);
    nova=root;
  }
  if(nova){
    if(haveValue){//todo:00 if node already initialized change value according to type. i.e. preserve node.type
      nova->setImageFrom(value.c_str(),Storable::Parsed);
//      if(valueQuoted){
//        //keep the text type set by setImage.
//      } else {//mark for further inspection by datum user.
//        //might be 'true' 'false' 'null' or some custom token
////if node is new this will be true, if not it already knows its type        nova->setType(Storable::Uncertain);//todo:0 too agressive, should preserve known types//mark for deferred interpretation
//      }
    } else {//either a trivial value (a formal json defect) or a parent
      if(valueQuoted){//empty quotes were encountered
        nova->setType(Storable::Textual);//#_# all we really want to signal here is 'not a keyword'
      } else {      //inferring wad node
        nova->setType(Storable::Wad);
      }
    }
  }
  return nova;
}

void StoreJsonConstructor::exclaim(PushedJSON::Parser::Diag &d) {
  wtf("Bad char 0x%02X at row:%u, col:%u, offset:%u",d.last,d.row,d.column,d.location);
}
