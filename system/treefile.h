#pragma once

#include "filename.h"
#include "storable.h"
#include "storednumeric.h" //version number

class TreeFile {
public:
  FileName filename;
  Storable &root;
  StoredReal svnnumber;

  TreeFile(TextKey fname, Storable &root);
  TreeFile(FileName &fname, Storable &root);

  bool parseTreeFile();
  bool printTree(bool blocking, bool debug = false);
/** version number of program which last wrote this file */
  double svn();
  //diagnostic convenience: make a file from any sub-tree
  static bool dumpStorage(Storable&root, const char *location);
}; // class TreeFile

/** build id of this binary used as a file format version */
double svn();
