#pragma once // (C) 2024 Andrew L. Heilveil, aka github/980F

#include <netinet/in.h> //because we template some stuff else this would have been only in the cpp file.
#include <unistd.h>
#include "fildes.h"

#include "sigcuser.h"

/** intermediate class for socket manipulation, mostly adding syntax to int fd usage */
struct IoSource : Fildes, SIGCTRACKABLE {

  IoSource(const char *traceName, int fd = ~0);
  /** note: will close the fd is marked as owner. */
  ~IoSource();

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

/** event manager for an IoSource */
class IoConnections : SIGCTRACKABLE {
public:
  IoConnections(IoSource &source);
  IoSource &source;

  using Slot = SimpleSlot;
  sigc::connection incoming;
  sigc::connection outgoing;
  sigc::connection hangup;
  /** drop all callback registrations.
   *  Note: that is automatic on destruction of each member, we don't need an explicit destructor.
   */
  void disconnect();
  /** call this when you have something to send.
   *  @returns whether it took some action to enable the write notification
   */
  bool writeInterest(Slot action);
  /** set listeners, was named hookup in a prior incarnation */
  void listen(Slot readAction, Slot hangupAction);
  // write slurpInput() -- calls input function until we run out of input
};
