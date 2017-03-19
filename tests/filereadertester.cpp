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
    received+=ret;

    buf.skip(ret);
    buf.clearUnused();
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
  freader(true/*read*/,fd,buf,  [this](__ssize_t arg){return this->onRead (arg);})
{

}

//note: 'test all' tests from the bottom up.
TextKey testfile[]={
  "filereadertester.1",
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
    buf.rewind();//when this was missing I learned things about how aio_read worked
    if(freader.go()){
      info("waiting for about %d events",blocksexpected);
      while(blocksin<blocksexpected){
        if(freader.block(1)){
          info("While waiting got: %d(%s)",freader.errornumber,strerror(freader.errornumber));
          if(freader.errornumber==EINTR){//on read or block shorter than buffer.
            if(received==expected){
              info("...which is pointless, happens in last incompletely filled block");
            }
          }
        }
        //todo: detect freader errornumber's that are fatal and break
      }
      info("Received: %ld of %ld, \tBlocks: %d of %d",received,expected,blocksin,blocksexpected);
    }
  }

}
