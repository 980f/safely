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


void StoredJSONparser::setValue(Storable &nova){
  Text value(data.internalBuffer(),parser.value);
  nova.setImage(value,Storable::Parsed);
  nova.setType(Storable::Uncertain);//mark for deferred interpretation
}

Storable *StoredJSONparser::assembleItem(Storable *parent){
  Storable *nova=nullptr;
  if (parser.haveName){
    Text name(data.internalBuffer(),parser.name);
    nova= insertNewChild(parent,name);
  } else {
    nova=insertNewChild(parent,"");
  }
  if(nova){
    setValue(*nova);
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
      for(Storable *nova = assembleItem(parent); parseChild(nova); ) {
        //#recurse while there are more to be found
      }
      return true; //end of this wad isn't the same as end of possibly enclosing wad.

    case EndItem:  //comma between siblings
      assembleItem(parent);
      return true;

    case EndWad:   //closing wad
      assembleItem(parent);
      return false;

    default:
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
