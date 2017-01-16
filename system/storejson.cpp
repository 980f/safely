#include "safely.h"
#include "storejson.h"
#include "textpointer.h"
#include "cheaptricks.h"

using namespace PushedJSON;


StoredJSONparser::StoredJSONparser(Indexer<const char> &loaded, Storable *root):data(loaded),root(root){
  //#nada
}


bool StoredJSONparser::parse(Storable *&root){
  parseChild(nullptr);
  root=this->root;
  return root!=nullptr;
}

Storable *StoredJSONparser::insertNewChild(Storable *parent,TextKey name){
  if(parent) {
    return &parent->addChild(name);
  } else { //is root node
    return root = new Storable(name);
  }
}

Storable *StoredJSONparser::assembleItem(Storable *parent,bool evenIfEmpty){
  Storable *nova=nullptr;
  bool novalue=parser.value.empty()&&!parser.wasQuoted;//empty string
  if(evenIfEmpty || !novalue){ //checking novalue here gets rids of extraneous ',' in the source
    if (parser.haveName){
      Text name(data.internalBuffer(),parser.name);
      nova= insertNewChild(parent,name);
    } else {
      nova=insertNewChild(parent,"");
    }
    if(nova){//pathological to not have one
      if(novalue){
        if(flagged(parser.wasQuoted)){
          s.onNode(true);
          nova->setType(Storable::Textual);//mark for deferred interpretation
        } else {      //inferring wad node
          s.onNode(false);
          nova->setType(Storable::Wad);//
        }
      } else {
        s.onNode(true);//even null nodes are deemed scalar.
        Text value(data.internalBuffer(),parser.value);
        //maydo: here is where we would process text escapes, but I'd rather not include all possible escape processors in this module.
        nova->setImage(value,Storable::Parsed);
        if(flagged(parser.wasQuoted)){
          //keep the text type set by setImage.
        } else {//mark for further inspection by datum user.
          nova->setType(Storable::Uncertain);//mark for deferred interpretation
        }
      }
    }
  }
  parser.itemCompleted();//ensure we don't reuse old data on next item.
  return nova;
}

bool StoredJSONparser::parseChild(Storable *parent){
  JsonStats::DepthTracker doe(s);
  //look for name
  while(data.hasNext()) {
    switch (parser.next(data.next())) {
    case Done://won't happen due to data.hasNext();
//    default:
      break;
    case Continue:
      break;
    case BeginWad: //open brace encountered
      for(Storable *nova = assembleItem(parent,true); parseChild(nova); ) {
        //#recurse while there are more to be found
      }
      return true; //end of this wad isn't the same as end of possibly enclosing wad.

    case EndItem:  //comma between siblings
      assembleItem(parent);
      return true;

    case EndWad:   //closing wad
      assembleItem(parent);
      return false;

    case Illegal: //unexpected char
      wtf("Bad char 0x%02X at row:%u, col:%u, offset:%u",parser.d.last,parser.d.row,parser.d.column,parser.d.location);
      break;

    } // switch
  }
  auto finial=parser.next(0);
  //at end of file we might have one last item, especially if we only have one item
  if(finial==EndItem){
    assembleItem(parent);
  }
  return false; //no more data so there are no more children,
} // StoredJSONparser::parseChild
