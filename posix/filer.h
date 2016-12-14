#ifndef FILER_H
#define FILER_H

#include "posixwrapper.h"
#include "fildes.h"
#include "buffer.h"
#include "charscanner.h" //bytescanner

/** file utilities, to keep Fildes class small.
  * may choose to rework as a derived class instead of containing one.*/
class Filer : public PosixWrapper {
  unsigned char *buffer;
  int size;
  Fildes fd;
public:
  Filer();
  ~Filer();
  /** see fcntl.h for O_flags. @returns true on a successful open. */
  bool openFile(const char *fname, int o_flags,bool makeDirs=false);
  FILE * getfp(const char *fargs=0);
  /** for when you need to close the file before this object goes out of scope:*/
  int close(void);
  bool readall(int maxalloc);
  ByteScanner contents();
public:
  /** copy file */
  static bool cp(const char *src,const char *target,bool dashf=true,bool dashr=false);
  /** make directory, including all parents if @param itsparent */
  static bool mkDirDashP(const char *path, bool itsparent=true);
  /** delete a file */
  static int rm(const char *name,bool dashf=false, bool dashr=false);

  static int makeUniqueName(const char *dirname);

  static bool exists(const char *pathname);
  /** if @param ok is true then file @param pathname is deleted AND NB: the pathname is freed! */
  static void killfileIfOK(bool ok,const char *pathname);
  /** this rename works in background */
  static void moveFile(const char *from,const char *to);
  /** this rename is blocking */
  static int mv(const char *src,const char *target);
};

#endif // FILER_H
