//"(C) Andrew L. Heilveil, 2017"
#include "filewriter.h"

#include "fcntlflags.h"
#include "logger.h"

static Logger bug("FileWriter",true);

FileWriter::FileWriter():
  fd("FileWriter"),
  fmover(false/*write*/,fd,buf){
  //#nada
}

FileWriter::~FileWriter(){
  //#nada
}

bool FileWriter::process(TextKey fname, const ByteScanner &source,unsigned blocksize){
  if(fd.open(fname,O_CREAT|O_WRONLY|O_TRUNC)){
    bug("Launching xfer of file %s",fname);
    buf=source;
    fmover.prepare(blocksize);
    if(fmover.go()){
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

unsigned FileWriter::remaining() const {
  return buf.freespace();
}
