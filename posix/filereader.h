#ifndef FILEREADER_H
#define FILEREADER_H

#include "fileasynchio.h"

class FileReader{
  u8 buffer[2049];//how shall we make this user programmable?-> malloc the buffer based in input block size from FileInfo (via caller)
  Fildes fd;
protected:
  ByteScanner buf;
//  ssize_t expected;
//  ssize_t received;
//  unsigned blocksin;
//  unsigned blocksexpected;

  FileAsyncAccess freader;

protected://starting with overloads, will replace with delegates once it is tested.
  virtual bool action();
  virtual void onCompletion();
public:
  FileReader();
  virtual ~FileReader();//ensure fildes is released.
  bool process(TextKey fname);
private:
  /* on incremental read complete */
//  bool onRead(__ssize_t ret);
};

#endif // FILEREADERTESTER_H
