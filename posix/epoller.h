#pragma once
// "(C) Andrew L. Heilveil, 2017"

#include "posixwrapper.h"
#include "sys/epoll.h" //for event type
/** wrap the epoll function set */
#include "buffer.h"
#include "stopwatch.h" //for time services

struct EpollHandler {
  virtual ~EpollHandler() = default;
  virtual void operator()(unsigned flags)=0;
  static void Thunk(const epoll_event& event) {
    (*static_cast<EpollHandler *>(event.data.ptr))(event.events);
  }
};

class Epoller : public PosixWrapper, EpollHandler {
  int epfd;

  void processList();
  void operator()(unsigned flags) override;
  /** data returned from wait():*/
  Indexer<epoll_event> waitlist;
  unsigned numEvents;
public:
  /** epoll_create. @param maxreport is the maximum #of triggered fd's to handle on any one wait call. */
  Epoller(unsigned maxreport);

  ~Epoller();

  /** automatically called by destructor */
  bool close();

  /** @returns whether the underlying epoller was successfully created */
  operator bool() const {
    return epfd >= 0;
  }

  bool watch(int fd, unsigned eventbits, EpollHandler &handler);

  bool modify(int fd, unsigned eventbits, EpollHandler &handler);

  bool remove(int fd);

  /** core of event loop.Must be called frequently. xOr register this guy's fd with your main application epoll loop, watching for read events. */
  bool loop(NanoSeconds timeout);

  /** add to higher level watcher */
  void registerWith(Epoller &myWatcher);
public:

  /** FYI: number of times wait() has been called, will wrap so mostly just for debug.*/
  unsigned waitcount = 0;

  /** when events are processed this clock is updated, reading the clock once per event wakeup. The event handlers then reference this rather than reading the clock which read is delayed by other event handlers. */
  StopWatch eventTime;
  //cached call to eventTime.elapsed(). All handlers should reference this as the best estimate of when the event occurred, rather than sampling the clock when the event handler gets called.
  NanoSeconds elapsed;

  /** for debug messages, prints out which flags are active in the epevs mask. */
  static void explain(unsigned epevs, Logger &explainTo = ::dbg); //todo:1 add logger argument defaulted to present hardcoded one.

};

#if 0 //man pagish stuff
EpollEvents
  EPOLLIN       request/report read event
  EPOLLPRI      higher priority data received, no info on how to access that, you must somehow know that via the fd.
  EPOLLOUT      request/report write event where write event means "you can send data"
  EPOLLERR      error happened
  EPOLLHUP      hangup (signalled) happened

  EPOLLRDNORM
  EPOLLRDBAND
  EPOLLWRNORM   same as EPOLLOUT.
  EPOLLWRBAND
  EPOLLMSG  deprecated/probably useless
  EPOLLRDHUP     shutdown on peer socket (remote hungup locally), there might still be data to read!

  EPOLLEXCLUSIVE config to limit spamming of a shared actual event to many fd-es
  EPOLLWAKEUP    something to do with wakeup sources, ignored in many *nices
  EPOLLONESHOT   config for autodisabling on event delivery, use ctl_mod to re-enable
  EPOLLET        config for edge triggered reporting

#endif // EPOLLER_H
