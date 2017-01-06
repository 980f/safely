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
  ++totalNodes;
  if(parent) {
    return &parent->addChild(name);
  } else { //is root node
    return root = new Storable(name);
  }
}

Storable *StoredJSONparser::assembleItem(Storable *parent,bool evenIfEmpty){
  Storable *nova=nullptr;
  bool novalue=parser.value.empty();
  if(evenIfEmpty || !novalue){
    if (parser.haveName){
      Text name(data.internalBuffer(),parser.name);
      nova= insertNewChild(parent,name);
    } else {
      nova=insertNewChild(parent,"");
    }
    if(nova){
      Text value(data.internalBuffer(),parser.value);
      //maydo: here is where we would process text escapes, but I'd rather not include all possible escape processors in this module.
      nova->setImage(value,Storable::Parsed);
      if(parser.quoted){
        parser.quoted=false;//keep the text type set by setImage.
      } else {//mark for further inspection by datum user.
        nova->setType(Storable::Uncertain);//mark for deferred interpretation
      }
    }
  }
  parser.itemCompleted();//ensure we don't reuse old data on next item.
  return nova;
}

bool StoredJSONparser::parseChild(Storable *parent){
  CountedLock doe(nested);//inc on create, dec on exit
  maxDepth.inspect(nested);//heuristics

  //look for name
  while(data.hasNext()) {
    switch (parser.next(data.next())) {
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
      wtf("Bad char 0x%02X at row:%u, col:%u, offset:%u(?=%u)",data.previous(),parser.row,parser.column,parser.location,data.ordinal());
      break;
    default:
      break;//to stifle warnings
    } // switch
  }
  auto finial=parser.next(0);
  //at end of file we might have one last item, especially if we only have one item
  if(finial==EndItem){
    assembleItem(parent);
  }
  return false; //no more data so there are no more children,
} // StoredJSONparser::parseChild
