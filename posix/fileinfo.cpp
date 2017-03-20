#include "fileinfo.h"
#include "logger.h"
#include "string.h"

FileInfo::FileInfo(TextKey filename){
  if(nonTrivial(filename)){
    from(filename);
  }
}

FileInfo::FileInfo(int fd){
  from(fd);
}

bool FileInfo::from(TextKey filename){
  if(failed(stat(filename,&finfo))){
    dbg("stat(%s) failed, errno:%d (%s)",filename,errornumber,strerror(errornumber));
    return false;
  } else {
    return true;
  }
}

bool FileInfo::from(int fd){
  return ok(fstat(fd,&finfo));
}
