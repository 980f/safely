#ifndef FILEINFO_H
#define FILEINFO_H "(C) Andrew L. Heilveil, 2017"

#include "sys/stat.h"
#include "posixwrapper.h"
#include "textkey.h"

class FileInfo: public PosixWrapper {
public:
  struct stat finfo;
  /** calls @see from on @param filename */
  FileInfo(TextKey filename);
  /** calls @see from on @param filename */
  FileInfo(int fd, const char *whatfor);
  /** @returns whether @param filename 's info was successfully acquired*/
  bool from(TextKey filename);
  /** @returns whether @param fd 's info was successfully acquired, file does NOT have to be open.*/
  bool from(int fd);

  operator bool()const {
    return isOk();
  }

  __off_t size()const {
    return finfo.st_size;
  }
  //add more inline functions to rename the sometimes vague stat member names.

};

#endif // FILEINFO_H
