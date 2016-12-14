#include "fdset.h"

#include "cheapTricks.h"

FDset::FDset(){
  maxfd = 0;
  clear();
}

void FDset::clear(){
  FD_ZERO(&group);
  maxfd = -1;
}

bool FDset::validFd(int fd){
  return fd >= 0 && fd < __FD_SETSIZE;
}

bool FDset::notTrivial() const {
  return maxfd >= 0;
}

bool FDset::includes(int fd) const {
  return validFd(fd) && FD_ISSET(fd, &group);
}

bool FDset::include(int fd){
  if(validFd(fd)) {
    FD_SET(fd, &group);
    if(fd > maxfd) {
      maxfd = fd;
    }
    return notTrivial();
  } else {
    return false;
  }
} /* include */

bool FDset::exclude(int fd){
  if(validFd(fd)) {
    FD_CLR(fd, &group);
    if(fd == maxfd) {
      while(maxfd-- > 0) {
        if(includes(maxfd)) {
          break;
        }
      }
    }
    return notTrivial();
  } else {
    return false;
  }
} /* exclude */

SelectorSet::SelectorSet(){
  timeout.tv_usec = 0;
  timeout.tv_sec = 1;
}

fd_set *SelectorSet::prepared(fd_set&fds, bool check){
  if(check) {
    fds = group;
    return &fds;
  } else {
    FD_ZERO(&fds); //# in case callers checks whatever this is post select() despite having said here that they would not.
    return 0;
  }
}


int SelectorSet::select(const char *rwe){
  return ::select(maxfd + 1,
                  prepared(readers, isPresent(rwe, 'r')),
                  prepared(writers, isPresent(rwe, 'w')),
                  prepared(troublers, isPresent(rwe, 'e')),
                  &timeout);
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

//end of file.
