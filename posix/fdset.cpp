#include "fdset.h"

#include "char.h"
#include "minimath.h" //splitter

FDset::FDset() {
  maxfd = 0;
  clear();
}

void FDset::clear() {
  FD_ZERO(&group);
  maxfd = -1;
}

bool FDset::validFd(int fd) {
  return fd >= 0 && fd < __FD_SETSIZE;
}

bool FDset::notTrivial() const {
  return maxfd >= 0;
}

bool FDset::includes(int fd) const {
  return validFd(fd) && FD_ISSET(fd, &group);
}

bool FDset::include(int fd) {
  if (validFd(fd)) {
    FD_SET(fd, &group);
    if (fd > maxfd) {
      maxfd = fd;
    }
    return notTrivial();
  } else {
    return false;
  }
} /* include */

bool FDset::exclude(int fd) {
  if (validFd(fd)) {
    FD_CLR(fd, &group);
    if (fd == maxfd) {
      while (maxfd-- > 0) {
        if (includes(maxfd)) {
          break;
        }
      }
    }
    return notTrivial();
  } else {
    return false;
  }
} /* exclude */

SelectorSet::SelectorSet() :
    PosixWrapper("SelectorSet") {
  // default timeout is 0, instant return if nothing of interest is active.
  timeout.tv_usec = 0;
  timeout.tv_sec = 0;
}

fd_set *SelectorSet::prepared(fd_set &fds, bool check) {
  if (check) {
    fds = group;
    return &fds;
  } else {
    FD_ZERO(&fds); // # in case caller checks whatever this is post select() despite having said here that they would not.
    return 0;
  }
}


int SelectorSet::select(const char *rwe) {
  int quantity = 0;
  if (okValue(quantity, ::select(maxfd + 1,
                          prepared(readers, isPresent(rwe, 'r')),
                          prepared(writers, isPresent(rwe, 'w')),
                          prepared(troublers, isPresent(rwe, 'e')),
                          &timeout))) {
    return quantity;
  }
  return quantity; // # keep separate for debug
}

bool SelectorSet::isReadable(int fd) const {
  return FD_ISSET(fd, &readers);
}

bool SelectorSet::isWritable(int fd) const {
  return FD_ISSET(fd, &writers);
}

bool SelectorSet::isTroubled(int fd) const {
  return FD_ISSET(fd, &troublers);
}

void SelectorSet::setTimeout(double seconds) {
  int wholeSeconds = splitter(seconds);
  timeout.tv_sec = wholeSeconds;
  timeout.tv_usec = int(1e6 * seconds);
}

// end of file.
