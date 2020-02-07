#ifndef INCREMENTALFILETRANSFER_H
#define INCREMENTALFILETRANSFER_H "(C) Andrew L. Heilveil, 2017"

#include "fildes.h" //our file <-> safe buffer routines
#include "posixwrapper.h"

class IncrementalFileTransfer: public PosixWrapper{
protected:
  /** transfer direction. */
  const bool amReader;//const until we can make sense of changing direction while open.
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

  virtual bool onEachBlock(__ssize_t amount);
  virtual void onDone();

public:
  IncrementalFileTransfer(bool reader, Fildes &fd, ByteScanner &buf);
  virtual ~IncrementalFileTransfer()=default;//#just squelching a gratuitous warning
  /** @param amount is file size for read, blocksize for write */
  void prepare(unsigned amount);
  /** actual chunk mover must call this at end of each incremental transfer */
  bool onChunk(__ssize_t amount);
};

#endif // INCREMENTALFILETRANSFER_H
