#pragma once // "(C) Andrew L. Heilveil, 2017"

#include "fileasynchio.h"

/** write file in background */
class FileWriter:FileAsyncAccess {

public:
  FileWriter();

  ~FileWriter() override = default; // ensures fildes is released/file closed.
  /** @returns whether file writing has begun. Will fail for things like bad filename. */
  bool process(TextKey fname, const ByteScanner &source, unsigned blocksize = 512);
  /** for polling, @returns number of bytes not yet definitely sent, they may have been passed to the OS but the OS hasn't told us that it has dealt with them.*/
  unsigned remaining() const;

private:
  /* on incremental write complete */
  // todo: where did code go?  bool onWrite(__ssize_t ret);
};
