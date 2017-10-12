#ifndef JSONFILE_H
#define JSONFILE_H "(C) Andrew L. Heilveil, 2017"

#include "../../safely/system/storable.h"
#include "textpointer.h"
#include "fildes.h"

/** read and write json file to/from Storable. */
class JsonFile {
  Storable &root;
public:
  Text loadedFrom;
public:
  JsonFile(Storable &node);
  int loadFile(Cstr thename);
  void printOn(Fildes &alreadyOpened,unsigned indent=0, bool showVolatiles=false);
  void printOn(Cstr somefile, unsigned indent=0, bool showVolatiles=false);
  Cstr originalFile();
  int reload();
};

#endif // JSONFILE_H
