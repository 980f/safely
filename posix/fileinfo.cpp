#include "fileinfo.h"
#include "logger.h"
#include "string.h"

FileInfo::FileInfo(TextKey filename):PosixWrapper (filename){
  if(nonTrivial(filename)){
    from(filename);
  }
}

FileInfo::FileInfo(int fd,const char *whatfor):PosixWrapper (whatfor){
  from(fd);
}

bool FileInfo::from(TextKey filename){
  if(failed(stat(filename,&finfo))){
    dbg("stat(%s) failed, errno:%d (%s)",filename,errornumber,errorText());
    return false;
  } else {
    return true;
  }
}

bool FileInfo::from(int fd){
  return ok(fstat(fd,&finfo));
}
