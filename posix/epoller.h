#ifndef EPOLLER_H
#define EPOLLER_H

#include "posixwrapper.h"
#include "sys/epoll.h" //for event type
/** wrap the epoll function set */
#include "buffer.h"
#include <functional> //for callback
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
  int numEvents;
  bool wait(int timeoutms);

  /** respond to an event report from wait: */
  static void exec(const epoll_event &ev);

  /** core of event loop */
  bool doEvents(int timeoutms);
  void explain(unsigned epevs);
};




#endif // EPOLLER_H
