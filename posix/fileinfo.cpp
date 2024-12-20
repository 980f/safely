//"(C) Andrew L. Heilveil, 2017"
#include "fileinfo.h"
#include "logger.h"


FileInfo::FileInfo(TextKey filename): PosixWrapper(filename) {
  if (nonTrivial(filename)) {
    from(filename);
  }
}

FileInfo::FileInfo(int fd, const char *whatfor): PosixWrapper(whatfor) {
  from(fd);
}

bool FileInfo::from(TextKey filename, bool noFollow) {
  if (failed(noFollow ? lstat(filename, &finfo) : stat(filename, &finfo))) {
    dbg("stat(%s) failed, errno:%d (%s)", filename, errornumber, errorText());
    return false;
  } else {
    return true;
  }
}

bool FileInfo::from(int fd) {
  return ok(fstat(fd, &finfo));
}
