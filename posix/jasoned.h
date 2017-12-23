#ifndef JASONED_H
#define JASONED_H

#include "jsonfile.h"

template <class StoredThing> struct Jasoned {
  StoredThing thing;//something derived from Stored
  JsonFile file;
  Jasoned(Storable &node,bool loadnow=true):
    thing(node),
    file(node){
    if(loadnow){
      load(node.name);
    }
  }

  int load(Cstr filename){
    return file.loadFile(filename);
  }

  void save(){
    file.printOn(file.originalFile(),0);
  }
};


#endif // JASONED_H
