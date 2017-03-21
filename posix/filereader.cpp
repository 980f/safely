#include "filereader.h"

#include "fcntlflags.h"
#include "fileinfo.h"

#include <cstdio>
#include "logger.h"
#include "string.h" //strerror

static Logger bug("FileReader",false);

bool FileReader::onRead(__ssize_t ret){
  if(ret>=0){//then it is # of bytes transferred
    ++blocksin;
    received+=ret;
    buf.skip(ret);
    action();

    if(expected>received){
      if(buf.freespace()==0){
        buf.rewind();
      }
      return true;
    } else {
      onCompletion();
      return false;
    }
  }
  return false;
}

bool FileReader::action(){
  bug("Received: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
  return false;
}

void FileReader::onCompletion(){
  bug("Completed: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
}

FileReader::FileReader():
  buf(buffer,sizeof(buffer)),
  freader(true/*read*/,fd,buf,  [this](__ssize_t arg){return this->onRead (arg);})
{
  //#nada
}


bool FileReader::process(TextKey fname){
  FileInfo finfo(fname);
  if(!finfo.isOk()){
    bug("stat(%s) failed, errno:%d (%s)",fname,finfo.errornumber,strerror(finfo.errornumber));
    return false;
  }
  expected=finfo.size();
  received=0;
  blocksin=0;
  blocksexpected= quanta(expected,buf.allocated());
  if(fd.open(fname,O_RDONLY | O_NONBLOCK | O_DIRECT)){
    bug("Launching read of file %s",fname);
    buf.rewind();//when this was missing I learned things about how aio_read worked
    if(freader.go()){
      return true;
    }
  }
  return false;
}
