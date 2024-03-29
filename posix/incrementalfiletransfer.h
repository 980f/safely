#ifndef INCREMENTALFILETRANSFER_H
#define INCREMENTALFILETRANSFER_H "(C) Andrew L. Heilveil, 2017,2024"

#include "fildes.h" //our file <-> safe buffer routines
#include "posixwrapper.h"
#include "charscanner.h"

class IncrementalFileTransfer: public PosixWrapper{
protected:
  /** transfer direction. */
  const bool amReader;//const, make a separate IFT object to move bytes in the other direction.
public:
  /** the file */
  Fildes &fd;
  /** a view into the data source or sink */
  ByteScanner &buf;
  /** progress report */
  ssize_t expected;
  ssize_t transferred;
  unsigned blockstransferred;
  unsigned blocksexpected;
  //called each time data had been delivered or removed from buf
  virtual bool onEachBlock(__ssize_t amount);
  //called when transfer is complete
  virtual void onDone();

public:
  IncrementalFileTransfer(bool reader, Fildes &fd, ByteScanner &buf);
  virtual ~IncrementalFileTransfer();
  /** @param amount is file size for read, blocksize for write, the size of buf is used for the other size involved */
  void prepare(unsigned amount);
  /** actual chunk mover must call this at end of each incremental transfer */
  bool onChunk(__ssize_t amount);
};

#endif // INCREMENTALFILETRANSFER_H
