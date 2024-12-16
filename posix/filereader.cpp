//"(C) Andrew L. Heilveil, 2017"
#include "filereader.h"

#include "fcntlflags.h"
#include "fileinfo.h"

// #include "logger.h"

static Logger bug("FileReader", false);

bool FileReader::action() {
  bug("Received: %ld of %ld, \tBlocks: %d of %d", freader.transferred, freader.expected, freader.blockstransferred, freader.blocksexpected);
  return true;
}

void FileReader::onCompletion() {
  bug("Completed: %ld of %ld, \tBlocks: %d of %d", freader.transferred, freader.expected, freader.blockstransferred, freader.blocksexpected);
}

FileReader::FileReader(): freader(true/*read*/) {
  //#nada
}

FileReader::~FileReader() {
  //#avert trivial warning about vtables.
}


bool FileReader::process(TextKey fname) {
  FileInfo finfo(fname);
  if (!finfo.isOk()) {
    bug("stat(%s) failed, errno:%d (%s)", fname, finfo.errornumber, finfo.errorText());
    return false;
  }
  freader.prepare(finfo.size());
  if (freader.fd.open(fname,O_RDONLY)) {
    bug("Launching read of file %s", fname);
    freader.buf.rewind(); //when this was missing I learned things about how aio_read worked :)
    if (freader.go()) {
      return true;
    }
  }
  return false;
}

void FileReader::loiter() {
  //todo:00 hang around until transfer is complete.
  //find out where original code went!
}
