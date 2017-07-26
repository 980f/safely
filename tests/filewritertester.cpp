#include "filewritertester.h"
#include "fcntlflags.h"
#include "fileinfo.h"
#include "filer.h"
#include "textpointer.h"

#include <cstdio>
#include "logger.h"

static Logger info("AIOFILE.WR",true);

/*bool FileWriterTester::action(){
  return true;
}

void FileWriterTester::onCompletion(){
  info("completed");
}*/

FileWriterTester::FileWriterTester(){

}

//note: 'test all' tests from the bottom up.
static TextKey testfile[]={
  "filereadertester.1",
  "filereadertester.0"
};


void FileWriterTester::run(unsigned which){
  if(which==BadIndex){
    for(which=countof(testfile);which-->0;){
      run(which);
    }
    return;
  }

  Filer testdata("FileWriterTester");
  testdata.openFile(testfile[which],O_RDONLY);
  if(testdata.readall(1000000)){
    ByteScanner data(testdata.contents());

// can't use  FILE *tfile=tmpfile(); as it is near impossible to access the filename to pass to a check routine.
    Text fname(tempnam(nullptr,nullptr));
    info("will write to: %s",fname.c_str());
    if(process(fname,data)){
      fmover.loiter();
    }
  }
}
