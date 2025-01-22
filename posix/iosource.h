#pragma once // (C) 2024 Andrew L. Heilveil, aka github/980F

#include <epoller.h>
#include <netinet/in.h> //because we template some stuff else this would have been only in the cpp file.
// #include <unistd.h>
#include "fildes.h"

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

  using IoEventHandler=std::function<void()>;
  IoEventHandler readAction;
  IoEventHandler writeAction;
  IoEventHandler closeAction;
  IoEventHandler errorAction;

//one watcher serves all iosources. This is similar to timer fd's. Each class needs its own epoller in order to recover the type of the object whose fd is being watched.
  static Epoller watcher;

public:

  /** drop all watching
   *  Note: that is automatic on destruction of each member, we don't need an explicit destructor.
   */
  void disconnect();


  /** call this when you have something to send.
   *
   */
  void writeInterest(bool postem=true);

  /** set listeners, was named hookup in a prior incarnation */
  void listen(bool postReads, bool postCloses);

  // write slurpInput() -- calls input function until we run out of input

  //epoller doesn't know about objects. It's idea of a callback has just 64 bits of data and that might be the size of a pointer.
  static void EventAdapter(void *iocon, unsigned flags) {
    if (iocon) {
      IoSource &ioc = *static_cast<IoSource *>(iocon);
      if (flags & EPOLLIN) {
        ioc.readAction();
      }
      if (flags & EPOLLOUT) {
        ioc.writeAction();
      }
      if (flags & EPOLLHUP) { //todo:1  fold RDHUP, other ERRORs into this?
        ioc.closeAction();
      }
      if (flags & (EPOLLERR)) {
        ioc.errorAction();
      }
    }
  }
};
