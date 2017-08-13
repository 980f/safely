#ifndef FILEREADER_H
#define FILEREADER_H "(C) Andrew L. Heilveil, 2017"

#include "fileasynchio.h"

class FileReader{
  u8 buffer[2049];//how shall we make this user programmable?-> malloc the buffer based in input block size from FileInfo (via caller)
  Fildes fd;
protected:
  ByteScanner buf;
  FileAsyncAccess freader;

protected://starting with overloads, will replace with delegates once it is tested.
  virtual bool action();
  virtual void onCompletion();
public:
  FileReader();
  virtual ~FileReader();//ensure fildes is released.
  bool process(TextKey fname);
  /** hang around until transfer seems complete.
   * implemented for module testing.  */
  void loiter();
};

#endif // FILEREADERTESTER_H
