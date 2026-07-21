#include "filereadertester.h"
//#include "fcntlflags.h"
#include "fileinfo.h"

#include "logger.h"
#include <array> //size of an array

static Logger info("AIOFILERD",true);

// bool FileReaderTester::action(){
//   buf.clearUnused();//low performance method of terminating a string to pass as a char *.
//   info("%s",buf.internalBuffer());
//   return true;
// }

void FileReaderTester::onDone(){
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
  if(which==BadIndex){//then do all of them
    for(which=std::size(testfile);which-->0;){
      run(which);
    }
    return;
  }
  TextKey fname=testfile[which];

  if(process(fname)){
    info("waiting for about %d events",blocksexpected);
    while(notDone()){
      if(block(1)){
        if(!isOk()){
          info("While waiting got: %d(%s)",errornumber,errorText());
          if(errornumber==EINTR){//on read or block shorter than buffer.
            if(transferred==expected){
              info("...which is pointless, happens in last incompletely filled block");
            }
          }
        }
      }
      //todo: detect freader errornumber's that are fatal and break
    }
  }
}
