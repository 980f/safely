#pragma once //"(C) Andrew L. Heilveil, 2017,2024"

#include "fileasynchio.h"
#include "charscanner.h"

class FileReader : public FileAsyncAccess {
  uint8_t buffer[2049]{}; //todo:1 how shall we make this user programmable?-> malloc the buffer based on input block size from FileInfo (via caller)

public:
  FileReader();

  bool process(TextKey fname);

  bool onEachBlock(ssize_t amount) override;
};
