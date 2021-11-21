//"(C) Andrew L. Heilveil, 2017"
#include "filewriter.h"

#include "fcntlflags.h"
#include "logger.h"

static Logger bug("FileWriter",false);

FileWriter::FileWriter() :
  fd("FileWriter"),
  fmover(false /*write*/,fd,buf){
  //#nada
}

bool FileWriter::process(TextKey fname, const ByteScanner &source,unsigned blocksize){
  if(fd.open(fname,O_REWRITE)) {
    bug("Launching xfer of file %s",fname);
    buf = source;
    fmover.prepare(blocksize);
    if(fmover.go()) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
} // FileWriter::process

unsigned FileWriter::remaining() const {
  return buf.freespace();
}
