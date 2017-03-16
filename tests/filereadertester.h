#ifndef FILEREADERTESTER_H
#define FILEREADERTESTER_H

#include "filereader.h"

class FileReaderTester{
  u8 buffer[2048];
  Fildes fd;
  ByteScanner buf;
  /* on read complete */
  bool onRead(size_t ret);
public:
  FileReaderTester();
  void run(unsigned which);
};

#endif // FILEREADERTESTER_H
