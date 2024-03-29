#ifndef FILEREADER_H
#define FILEREADER_H "(C) Andrew L. Heilveil, 2017,2024"

#include "fileasynchio.h"
#include "charscanner.h"

class FileReader{
  u8 buffer[2049];//todo:1 how shall we make this user programmable?-> malloc the buffer based on input block size from FileInfo (via caller)
  Fildes fd;
protected:
  ByteScanner buf;
  FileAsyncAccess freader;

protected://starting with overloads, will replace with delegates once it is tested.
  virtual bool action();
  virtual void onCompletion();
public:
  FileReader();
  virtual ~FileReader();//ensures fildes is released.
  bool process(TextKey fname);
  /** hang around until transfer seems complete.
   * implemented for module testing.  */
  void loiter();
};

#endif // FILEREADERTESTER_H
