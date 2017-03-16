#include "filereadertester.h"

#include "sigcuser.h"

#include <cstdio>

bool FileReaderTester::onRead(size_t ret)
{
  if(ret>=0){
    buf[ret]=0;//null terminate, will fail if file is greater than buffer ...
    printf("%s\n",buf.internalBuffer());
  }
}

FileReaderTester::FileReaderTester():buf(buffer,sizeof(buffer)){

}

bool freadreport(size_t ret){
  printf("Completion code: %ld",ret);
}

void FileReaderTester::run(unsigned which){
  if(which==~0){
    for(which=1;which-->0;){
      run(which);
    }
    return;
  }
  switch(which){
  case 0:{
      fd.open("filereadertester.0",0);

      FileReader freader(fd,buf,&freadreport);
                         //MyHandler(FileReaderTester::onRead));
      freader(1);
    } break;
  }
}
