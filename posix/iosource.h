#pragma once // (C) 2024 Andrew L. Heilveil, aka github/980F

#include <epoller.h>
#include <netinet/in.h> //because we template some stuff else this would have been only in the cpp file.
// #include <unistd.h>
#include <functional>

#include "fildes.h"

#ifndef SafelyIoSourceEvents
#define SafelyIoSourceEvents 7
#warning "using default Io events per poll value, define SafelyIoSourceEvents to optimize space (low value) or performance (frequent IO)"
#endif

/** intermediate class for socket manipulation, mostly adding syntax to int fd usage. */
struct IoSource : Fildes {
  IoSource(const char *traceName, int fd = ~0);

  /** note: will close the fd if marked as owner. */
  ~IoSource() override;

  /** merge return from read or write with errno, @returns negative errno for most errors, 0 for errors worthy of simple retry (same as 0 bytes read or written) else the number of bytes successfully operated upon */
  static int recode(ssize_t rwreturn);

  template<typename OptionArgument> bool setOption(int optFamily, int optname, OptionArgument optval) {
    return ok(setsockopt(fd, optFamily, optname, &optval, sizeof(OptionArgument)));
  }

  template<typename OptionArgument> bool getOption(int optFamily, int optname, OptionArgument &optval) {
    socklen_t length(sizeof(OptionArgument));
    return ok(getsockopt(fd, optFamily, optname, &optval, &length));
  }
};

struct IoAgent : IoSource, EpollHandler {
  /* given a name, and a epoll resource and a possibly open fd */
  IoAgent(const char *traceName, EpollerCore &watcher, int fd = BADFD) : IoSource{traceName, fd}, watcher{watcher} {}
  EpollerCore &watcher;
  //formerly in a wrapping class, but there is no value to the separation.
  unsigned epollFlags = 0;
  using IoEventHandler = std::function<void()>;
  IoEventHandler readAction;
  IoEventHandler writeAction;
  IoEventHandler closeAction;
  IoEventHandler errorAction;

public:
  /** drop all watching
   *  Note: that is automatic on destruction of each member, we don't need an explicit destructor.
   */
  void disconnect();

  void setWatching(unsigned flag, bool postem);

  /** call this when you have something to send.
   *
   */
  void writeInterest(bool postem = true) {
    setWatching(EPOLLOUT, postem);
  }

  /** set listeners, was named hookup in a prior incarnation */
  void listen(bool postReads, bool postCloses);

  // write slurpInput() -- calls input function until we run out of input

  //epoller doesn't know about objects. Its idea of a callback has just 64 bits of data and that might be the size of a pointer.
  void onEpoll(unsigned flags) override {
    if (flags & EPOLLIN) {
      readAction();
    }
    if (flags & EPOLLOUT) {
      writeAction();
    }
    if (flags & EPOLLRDHUP) {
      closeAction();
    }
    if (flags & EPOLLERR) {
      errorAction();
    }
  }

  ~IoAgent() override;
};
