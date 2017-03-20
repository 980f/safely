#include "filereadertester.h"
#include <fcntl.h> //O_options, need to enum them
#include "logger.h"

bool FileReaderTester::onRead(__ssize_t ret){
  if(ret>=0){
    buf[ret]=0;//null terminate, will fail if file is greater than buffer ...
    dbg("%s\n",buf.internalBuffer());
  }
  return false;
}

FileReaderTester::FileReaderTester():
  buf(buffer,sizeof(buffer)),
  freader(fd,buf,  [this](__ssize_t arg){return this->onRead (arg);})
{

}


void FileReaderTester::run(unsigned which){
  if(which==BadIndex){
    for(which=1;which-->0;){
      run(which);
    }
    return;
  }
  switch(which){
  case 0:{
      if(fd.open("filereadertester.0",O_RDONLY)){
        if(freader(1)){
          freader.block();
        }
      }
    } break;
  }
}
