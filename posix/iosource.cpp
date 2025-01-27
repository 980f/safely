#include "iosource.h"

IoSource::IoSource(const char *tracename, int fd): Fildes(tracename) {
  if (fd >= 0) {
    preopened(fd, false); //if preopened we don't own it. But creator of this IoSource can pass ownership to it later.
  }
}

IoSource::~IoSource() {
  //make sure Fildes destructor is called.
}

int IoSource::recode(ssize_t rwreturn) {
  if (rwreturn == ~0) {
    int errnum(errno);
    if (EINTR == errnum || EAGAIN == errnum || EWOULDBLOCK == errnum) {
      return 0; // still more perhaps.
    }
    return -errnum;
  }
  return rwreturn;
}
///////////////////////////////////////////////////////////
// #ifndef SafelyIoAgentMax
// #define SafelyIoAgentMax 20
// #warning "SafelyIoAgentMax: setting soft maximum number of active event driven IO streams to 20, you will need at least one per socket connection to ensure no starvation of I/O handling."
// #endif
// Epoller IoAgent::watcher(SafelyIoAgentMax);

void IoAgent::disconnect() {
  watcher.remove(fd);
  epollFlags = 0;
}

void IoAgent::setWatching(unsigned flag, bool postem) {
  if (epollFlags == 0) {
    watcher.watch(fd, flag, *this);
    epollFlags = flag;
  } else {
    if (postem) {
      epollFlags |= flag;
    } else {
      epollFlags &= ~flag;
    }
    if (epollFlags==0) {
      watcher.remove(fd);
    }
    watcher.modify(fd, epollFlags, *this);
  }
}

void IoAgent::listen(bool postReads, bool postCloses) {
  setWatching(EPOLLIN, postReads);
  setWatching(EPOLLRDHUP, postCloses);
}

IoAgent::~IoAgent() {
  disconnect();//nicely this gets done before base class close(), averting having to read the documentation on epoll and closed fd's still having readable data.
}
