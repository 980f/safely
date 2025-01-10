#ifndef FILEREADERTESTER_H
#define FILEREADERTESTER_H

#include "filereader.h"

class FileReaderTester: public FileReader{
  // bool action() override;
  // void onCompletion() override;
public:
  FileReaderTester();
  void run(unsigned which);
};

#endif // FILEREADERTESTER_H
