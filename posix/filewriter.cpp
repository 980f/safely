#include "filewriter.h"

#include "fcntlflags.h"
#include "fileinfo.h"

#include <cstdio>
#include "logger.h"
#include "string.h" //strerror

static Logger bug("FileWriter",true);

bool FileWriter::onWrite(__ssize_t ret){
  if(ret>=0){//then it is # of bytes transferred
    ++blocksout;
    sent+=ret;
    buf.skip(ret);
    action();

    if(buf.freespace()){
      return true;
    } else {
      onCompletion();
      return false;
    }
  }
  return false;
}

bool FileWriter::action(){
//  bug("Received: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
  return false;
}

void FileWriter::onCompletion(){
//  bug("Completed: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
}

FileWriter::FileWriter():
  freader(false/*write*/,fd,buf,  [this](__ssize_t arg){return this->onWrite (arg);})
{
  //#nada
}

bool FileWriter::process(TextKey fname, const ByteScanner &source){
  if(fd.open(fname,O_CREAT|O_WRONLY|O_TRUNC|O_NONBLOCK)){
    bug("Launching read of file %s",fname);
    buf=source;
    if(freader.go()){
      return true;
    }
  }
  return false;
}

unsigned FileWriter::remaining() const {
  return buf.freespace();
}
