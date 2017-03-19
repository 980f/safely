#include "filereadertester.h"

#include "fcntlflags.h"
#include "sys/stat.h"
#include <cstdio>
#include "logger.h"
#include "string.h" //strerror

static Logger info("AIOFILE",true);

bool FileReaderTester::onRead(__ssize_t ret){
  if(ret>=0){
    ++blocksin;
    buf.skip(ret);
    received+=buf.used();

    buf[ret]=0;//null terminate, will fail if file is greater than buffer ...
    info("%s",buf.internalBuffer());
    if(expected>received){
      buf.rewind();
      return true;
    }
  }
  return false;
}

FileReaderTester::FileReaderTester():
  buf(buffer,sizeof(buffer)),
  freader(fd,buf,  [this](__ssize_t arg){return this->onRead (arg);})
{

}


TextKey testfile[]={
  "/d/work/safely/tests/filereadertester.1",
  "filereadertester.0"
};

void FileReaderTester::run(unsigned which){
  if(which==BadIndex){
    for(which=countof(testfile);which-->0;){
      run(which);
    }
    return;
  }
  TextKey fname=testfile[which];

  struct stat finfo;
  int posixerr=stat(fname,&finfo);
  if(posixerr){
    info("stat(%s) failed, errno:%d (%s)",fname,errno,strerror(errno));
    return;
  }
  expected=finfo.st_size;
  received=0;
  blocksin=0;
  blocksexpected= quanta(expected,buf.allocated());
  if(fd.open(fname,O_RDONLY)){
    info("Launching read of file %s",fname);
    if(freader.go(1)){
      info("waiting for about %d events",blocksexpected);
      while(blocksin<blocksexpected){
        if(freader.block(1)){
          info("While waiting got: %d(%d)",freader.errornumber,strerror(freader.errornumber));
        }
        //todo: detect freader errornumber's that are fatal and break
      }
      info("Received: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
    }
  }

}
