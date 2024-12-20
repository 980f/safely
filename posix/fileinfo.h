#pragma once
// "(C) Andrew L. Heilveil, 2017"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <linux/limits.h>

#include "sys/stat.h"
#include "posixwrapper.h"
#include "textkey.h"
#include"textpointer.h"

class FileInfo : public PosixWrapper {
public:
  struct stat finfo;

  /** calls @see from on @param filename */
  FileInfo(TextKey filename);

  /** calls @see from on @param fd . @param whatfor is passed to debug log */
  FileInfo(int fd, const char *whatfor = "FileInfo");

  /** @returns whether @param filename 's info was successfully acquired*/
  bool from(TextKey filename, bool noFollow = false);

  /** @returns whether @param fd 's info was successfully acquired, file does NOT have to be open.*/
  bool from(int fd);

  operator bool() const {
    return isOk();
  }

  __off_t size() const {
    return finfo.st_size;
  }

  /** @returns whether this file has a link somewhere in its name.*/
  bool isLinked() const {
    return finfo.st_nlink > 0;
  }

  /** @return the 'real' name of what the given name linked to, with 'this' having the other attributes of the link itself */
  Text getLink(TextKey filename) {
    if (from(filename, true)) {
      /* Add one to the link size, so that we can determine whether the buffer returned by readlink() was truncated.
         Some magic symlinks under (for example) /proc and /sys report 'st_size' as zero. In that case, take PATH_MAX as a "hope this works" estimate. */
      auto bufsiz = (finfo.st_size > 0 ? finfo.st_size : PATH_MAX) + 1;
      char *target = static_cast<char *>(malloc(bufsiz));
      if (target) {
        auto nbytes = readlink(filename, target, bufsiz);
        if (nbytes >= 0) {
          target[nbytes] = 0;
        } else {
          target[bufsiz] = 0; //to hae a chance at debugging what happened.
        }
        return Text(target, true);
      } else {
        failure(ENOMEM);
        return Text();
      }
    }
  }
};
