#ifndef FILEREADERTESTER_H
#define FILEREADERTESTER_H

#include "filereader.h"

class FileReaderTester{
  u8 buffer[2049];
  Fildes fd;
  ByteScanner buf;
  ssize_t expected;
  ssize_t received;
  unsigned blocksin;
  unsigned blocksexpected;
  FileAsyncAccess freader;

  /* on read complete */
  bool onRead(__ssize_t ret);
public:
  FileReaderTester();
  void run(unsigned which);
};

#endif // FILEREADERTESTER_H
