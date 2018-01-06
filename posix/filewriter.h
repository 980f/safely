#ifndef FILEWRITER_H
#define FILEWRITER_H "(C) Andrew L. Heilveil, 2017"

#include "fileasynchio.h"

/** write file in background */
class FileWriter{
  Fildes fd;
protected:
  ByteScanner buf;
public: //should probably derive from it.
  FileAsyncAccess fmover;
public:
  FileWriter();
  virtual ~FileWriter();//ensure fildes is released.
  /** @returns whether file writing has begun. Will fail for things like bad filename. */
  bool process(TextKey fname, const ByteScanner &source, unsigned blocksize=512);
  /** for polling, @returns number of bytes not yet definitely sent, they may have been passed to the OS but the OS hasn't told us that it has dealt with them.*/
  unsigned remaining()const;
private:
  /* on incremental write complete */
//todo: where did code go?  bool onWrite(__ssize_t ret);
};

#endif
