#include "safely.h"
#include "storejson.h"
#include "textpointer.h"
#include "cheaptricks.h"


StoreJsonParser::StoreJsonParser(Indexer<const char> &data):AbstractJSONparser(core),core(data){
  //default inits of members NOT happening!
  stats.reset();
  parser.reset(true);
}

StoreJsonConstructor::StoreJsonConstructor(Indexer<const char> &data):
  data(data){
  //#nada
}

Text StoreJsonConstructor::extract(Span &span) {
  return Text(data.internalBuffer(),span);
}

Storable *StoreJsonConstructor::insertNewChild(Storable *parent, Text &name, bool haveValue, Text &value, bool valueQuoted) {
  Storable *nova=parent? &parent->child(name): (root = new Storable(name));
  if(nova){
    if(haveValue){
      nova->setImage(value,Storable::Parsed);
      if(valueQuoted){
        //keep the text type set by setImage.
      } else {//mark for further inspection by datum user.
        //might be 'true' 'false' 'null' or some custom token
        nova->setType(Storable::Uncertain);//mark for deferred interpretation
      }
    } else {//either a trivial value (a formal json defect) or a parent
      if(valueQuoted){
        nova->setType(Storable::Textual);
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
