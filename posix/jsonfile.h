#ifndef JSONFILE_H
#define JSONFILE_H

#include "storable.h"
#include "textpointer.h"
#include "fildes.h"
class JsonFile {
  Storable &root;
public:
  Text loadedFrom;
public:
  JsonFile(Storable &node);
  int loadFile(Cstr thename);
  void printOn(Fildes &alreadyOpened,unsigned indent=0);
  void printOn(Cstr somefile,unsigned indent=0);
  Cstr originalFile();
};

#endif // JSONFILE_H
