#include "storejson.h"
#include "textpointer.h"
#include "cheaptricks.h"

using namespace PushedJSON;

bool StoredJSONparser::parse(){
  parseChild(nullptr);
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

Storable *StoredJSONparser::makeNamelessChild(Storable *parent){
  return insertNewChild(parent,"");
}

Storable *StoredJSONparser::makeChild(Storable *parent){
  Text name(data.internalBuffer(),parser.name);
  parser.haveName = false;
  return insertNewChild(parent,name);
}

void StoredJSONparser::setValue(Storable &nova){
  Text value(data.internalBuffer(),parser.value);
  nova.setImage(value,Storable::Parsed);
  nova.setType(Storable::Uncertain);//mark for deferred interpretation
}

Storable *StoredJSONparser::assembleItem(Storable *parent){
  if(parser.haveName) {
    return makeChild(parent);
  } else {
    return makeNamelessChild(parent);
  }
}

bool StoredJSONparser::parseChild(Storable *parent){
  CountedLock doe(nested);//inc on create, dec on exit
  maxDepth.inspect(nested);//heuristics

  //look for name
  while(data.hasNext()) {
    Action action = parser.next(data.next());

    switch (action) {
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
  //at end of file we might have one last item, especially if we only have one item
  if(false){//todo: figure out how to detect well formed hanging fire.
    assembleItem(parent);
  }
  return false;
} // StoredJSONparser::parseChild
