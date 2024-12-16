//"(C) Andrew L. Heilveil, 2017"
#include "filewriter.h"

#include "fcntlflags.h"
#include "logger.h"

static Logger bug("FileWriter", false);

FileWriter::FileWriter() : FileAsyncAccess(false /*write*/) {}

bool FileWriter::process(TextKey fname, const ByteScanner &source, unsigned blocksize) {
  if (fd.open(fname, O_REWRITE)) {
    bug("Launching xfer of file %s", fname);
    buf = source;
    prepare(blocksize);
    return go();
  } else {
    return false;
  }
} // FileWriter::process

unsigned FileWriter::remaining() const {
  return buf.freespace();
}
