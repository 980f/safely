#ifndef EPOLLER_H
#define EPOLLER_H

#include "posixwrapper.h"
#include "sys/epoll.h" //for event type
/** wrap the epoll function set */
#include "buffer.h"

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

  bool add(int fd,struct epoll_event *event);
  bool modify(int fd,struct epoll_event *event);
  bool remove(int fd);
  /** data returned from wait():*/
  Indexer<struct epoll_event> waitlist;
  bool wait(int timeoutms);
};

#endif // EPOLLER_H
