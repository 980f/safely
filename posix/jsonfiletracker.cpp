#include "jsonfiletracker.h"
#include "bitbanger.h"

void JsonFileTracker::reload(const FileEvent &fe){
  if(BitWad<FileWatch::Attributes>::all(fe.mask)){//ignore delete and reads etc.
    jfile.reload();
  }
}

JsonFileTracker::JsonFileTracker(Storable &root):
  jfile(root){
  //#nada, not enough info to do anything more.
}

int JsonFileTracker::load(Cstr filename){
  wd=watcher.addWatch(filename,BitWad<FileWatch::CloseWrite,FileWatch::MovedTo>::mask);
  return jfile.loadFile(filename);
}
