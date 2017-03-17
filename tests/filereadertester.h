#ifndef FILEREADERTESTER_H
#define FILEREADERTESTER_H

#include "filereader.h"

class FileReaderTester{
  u8 buffer[2048];
  Fildes fd;
  ByteScanner buf;
  FileReader freader;

  /* on read complete */
  bool onRead(__ssize_t ret);
public:
  FileReaderTester();
  void run(unsigned which);
};

#endif // FILEREADERTESTER_H
