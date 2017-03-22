#include "filereadertester.h"
#include "fcntlflags.h"
#include "fileinfo.h"

#include <cstdio>
#include "logger.h"

static Logger info("AIOFILERD",true);

bool FileReaderTester::action(){
  buf.clearUnused();//low performance method of terminating a string to pass as a char *.
  info("%s",buf.internalBuffer());
  return true;
}

void FileReaderTester::onCompletion(){
  info("completed");
}

FileReaderTester::FileReaderTester(){
  buf.zguard(1);//ascii strings
}

//note: 'test all' tests from the bottom up.
static TextKey testfile[]={
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

  if(process(fname)){
    info("waiting for about %d events",blocksexpected);
    while(!freader.isDone() && blocksin<blocksexpected){
      if(freader.block(1)){
        info("While waiting got: %d(%s)",freader.errornumber,freader.errorText());
        if(freader.errornumber==EINTR){//on read or block shorter than buffer.
          if(received==expected){
            info("...which is pointless, happens in last incompletely filled block");
          }
        }
      }
      //todo: detect freader errornumber's that are fatal and break
    }
  }
}
