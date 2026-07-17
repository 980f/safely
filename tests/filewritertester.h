#ifndef FILEWRITERTESTER_H
#define FILEWRITERTESTER_H

#include "filewriter.h"

class FileWriterTester: public FileWriter{
//  bool action() override;
//  void onCompletion() override;
public:
  bool action() ;
  void onCompletion();
  FileWriterTester();
  void run(unsigned which);
};

#endif // FILEREADERTESTER_H
