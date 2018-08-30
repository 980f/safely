#ifndef FILER_H
#define FILER_H "(C) Copyright Andy Heilveil 2017-2018"

#include "posixwrapper.h"
#include "fildes.h"
#include "buffer.h"
#include "charscanner.h" //bytescanner

/** file utilities, to keep Fildes class small.
  * may choose to rework as a derived class instead of containing one.*/
class Filer : public PosixWrapper {
  unsigned char *buffer;
  unsigned size;
  Fildes fd;
public:
  Filer(const char *whatfor);
  ~Filer();
  /** see fcntl.h for O_flags. @returns true on a successful open. Note:0 if flags for simple read */
  bool openFile(const char *fname, int o_flags=0,bool makeDirs=false);
  /** get stream, @parm fargs is what you typically pass to fopen() */
  FILE * getfp(const char *fargs=0);
  /** for when you need to close the file before this object goes out of scope:*/
  int close(void);
  /** allocate a buffer that can hold the whole file and read it in.
   * File must already be successfully opened (so that we don't have to deal with those errors here).
   * if you read some from the file before calling this the buffer will be big enough for the whole file even though only the remainder will be read in. */
  bool readall(unsigned maxalloc=BadLength);//default arg: effectivley unlimited

  ByteScanner contents();
public:
  /** copy file */
  static bool cp(const char *src,const char *target,bool dashf=true,bool dashr=false);
  /** make directory, including all parents if @param itsparent */
  static bool mkDirDashP(const char *path, bool itsparent=true);
  /** delete a file */
  static int rm(const char *name,bool dashf=false, bool dashr=false);

  static bool exists(const char *pathname);

  /** this rename is blocking */
  static int mv(const char *src,const char *target);
};

#endif // FILER_H
