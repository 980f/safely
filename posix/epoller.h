#ifndef EPOLLER_H
#define EPOLLER_H "(C) Andrew L. Heilveil, 2017"

#include "posixwrapper.h"
#include "sys/epoll.h" //for event type
/** wrap the epoll function set */
#include "buffer.h"
#include <functional> //for callback

#include "stopwatch.h" //for time services
class Epoller:public PosixWrapper {
  int epfd;

public:
  /** epoll_create. @param maxreport is the maximum #of triggered fd's to handle on any one wait call. */
  Epoller(unsigned maxreport);
  ~Epoller();
  /** automatically called by destructor */
  bool close();
  /** @returns whether the underlying epoller was successfully created */
  operator bool()const{
    return epfd>=0;
  }
  using Handler=std::function<void(unsigned /*eventbits*/)>;

  bool watch(int fd, unsigned eventbits,Handler handler);
  bool modify(int fd, unsigned eventbits,Handler handler);
  bool remove(int fd);
  /** data returned from wait():*/
  Indexer<epoll_event> waitlist;
  unsigned numEvents;
  /** FYI: number of times wait() has been called, will wrap so mostly just for debug.*/
  unsigned waitcount=0;
  bool wait(unsigned timeoutms);

  /** respond to an event report from wait: */
  static void exec(const epoll_event &ev);

  /** core of event loop */
  bool doEvents(unsigned timeoutms);
  void explain(unsigned epevs);

  /** when events are processed this clock is updated, reading the clock once per event wakeup */
  StopWatch eventTime;
  //cached call to eventTime.elapsed(). If someone screws it up it will be fixed on next event.
  double elapsed;


};




#endif // EPOLLER_H
