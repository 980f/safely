#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "dirent.h"
#include "textpointer.h"
#include "posixwrapper.h"

/** wrapper around use of dirent and DIR posix structs */
class Directory: public PosixWrapper {
  Text base;
  DIR *dirp;
public:
  Directory(const char * base);
  dirent *next();
};

#endif // DIRECTORY_H
