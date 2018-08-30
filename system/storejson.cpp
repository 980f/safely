//"(C) Andrew L. Heilveil, 2017"
#include "storejson.h"
#include "textpointer.h"
#include "cheaptricks.h"

#include "logger.h"

#if DebugSafelyItself
SafeLogger(jsonspew,true);
#else
#define jsonspew(...)
#endif

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
    if(parent->parserstate!=BadIndex){//then it is index of node to add/overwrite
      jsonspew("adding nth %d to %s",parent->parserstate,parent->name.c_str());
      nova=&parent->nth(parent->parserstate,true);
      ++parent->parserstate;
    } else {
      if(name.empty()){
        jsonspew("adding nameless child to %s",parent->name.c_str());
        nova=&parent->addChild(nullptr);//typically an array element, do NOT make all nameless entities the same entity.
      } else {
        jsonspew("adding child %s to %s",name.c_str(),parent->name.c_str());
        nova=parent->findChild(name,true);
      }
    }
  } else {
    jsonspew("adding root %s",name.c_str());
    root = &Storable::Groot(name);
    nova=root;
  }
  if(nova){
    if(haveValue){
      jsonspew("node %s set value to %s",nova->name.c_str(),value.c_str());
      nova->setImageFrom(value.c_str(),Storable::Parsed);
    } else {//either a trivial value (a formal json defect) or a parent
      if(valueQuoted){//empty quotes were encountered
        //the following was legacy from when the loader tried to guess types. If we honor null/true/false keywords we need to add a function to storable for that.
//        nova->setType(Storable::Textual);//#_# all we really want to signal here is 'not a keyword'
      } else {      //inferring wad node
        jsonspew("node %s seems to be a wad",nova->name.c_str());
        nova->setType(Storable::Wad);
      }
    }
  } else {
    jsonspew("could not make a node for %s:%s",name.c_str(),value.c_str());
  }
  return nova;
}

void StoreJsonConstructor::exclaim(PushedJSON::Parser::Diag &d) {
  wtf("Bad char 0x%02X at row:%u, col:%u, offset:%u",d.last,d.row,d.column,d.location);
}
