//"(C) Andrew L. Heilveil, 2017"
#include "filereader.h"

#include "fcntlflags.h"
#include "fileinfo.h"

static Logger bug("FileReader", false);

//
// void FileReader::onDone() {
//   bug("Completed: %ld of %ld, \tBlocks: %d of %d", transferred, expected, blockstransferred, blocksexpected);
// }

FileReader::FileReader(): FileAsyncAccess(true/*read*/) {
  EraseThing(buffer);//4debug
}



bool FileReader::process(TextKey fname) {
  FileInfo finfo(fname);
  if (!finfo.isOk()) {
    bug("stat(%s) failed, errno:%d (%s)", fname, finfo.errornumber, finfo.errorText());
    return false;
  }
  prepare(finfo.size());
  if (fd.open(fname,O_RDONLY)) {
    bug("Launching read of file %s", fname);
    buf.rewind(); //when this was missing I learned things about how aio_read worked :)
    if (go()) {
      return true;
    }
  }
  return false;
}

//we expect this to be overridden frequently by something that reads and rewinds the buffer.
bool FileReader::onEachBlock(ssize_t amount) {
  bug("Received: %zu, total: %ld of %ld, \tBlocks: %d of %d", amount, transferred, expected, blockstransferred, blocksexpected);
  return true;
}
