//"(C) Andrew L. Heilveil, 2017-2018"

#include "EpollerCore.h"


#include <sys/epoll.h>
#include <unistd.h>

#include "logger.h"

//constexpr unsigned evlistsize(unsigned number){
//  return sizeof (epoll_event)*number;
//}

EpollerCore::EpollerCore(const char *tracename): PosixWrapper(tracename) ,
  epfd(-1),
  // BuildIndexer(epoll_event, waitlist, maxreport),
  numEvents(BadLength), //init for debug
  eventTime(true, true) //use real time, process may sleep
{
  epfd = epoll_create1(0);
}

EpollerCore::~EpollerCore() {
  close();
}

bool EpollerCore::close() {
  if (*this) { //checking for debug purpose, avoid recording irrelevant error
    return ok(::close(epfd));
  } else {
    return true;
  }
}

bool EpollerCore::watch(int fd, unsigned eventbits, EpollHandler &handler) {
  epoll_event event{eventbits, {&handler}};
  return ok(epoll_ctl(epfd,EPOLL_CTL_ADD, fd, &event));
}

bool EpollerCore::modify(int fd, unsigned eventbits, EpollHandler &handler) {
  epoll_event event{eventbits, {&handler}};

  return ok(epoll_ctl(epfd,EPOLL_CTL_MOD, fd, &event));
}

bool EpollerCore::remove(int fd) {
  return ok(epoll_ctl(epfd,EPOLL_CTL_DEL, fd, nullptr));
}


void EpollerCore::processList() {
  Indexer list = waitlist.takeHead(); //get head of list
  while (list.hasNext()) {
    auto event = list.next();
    //dbg("event on fd: %d",event.data.fd);
    if (event.events == 0 || !event.data.ptr) {
      //we shouldn't be here.
      wtf("got a no-event response");
    } else {
      Thunk(event);
    }
  }
}

//this guy gets called when a master epoller has found a change in it, and instantly returns so we don't do all the reactTime and tieout stuff of @see loop()
void EpollerCore::operator()(unsigned flags) {
  auto list = waitlist.getTail();
  ++waitcount;
  if (okValue(numEvents, unsigned(epoll_pwait2(epfd, list.internalBuffer(), list.allocated(), nullptr, nullptr)))) {
    waitlist.skip(numEvents);
    processList();
  }
}

bool EpollerCore::loop(NanoSeconds timeout) {
  static StopWatch reactionTime(false, true); //diagnostic: perhaps using the wrong timebase caused epoller to not wait?
  static unsigned long shortwait = 0; //number of short waits, a diagnostic.
  reactionTime.start();

  auto list = waitlist.getTail();
  ++waitcount;
  if (okValue(numEvents, unsigned(epoll_pwait2(epfd, list.internalBuffer(), list.allocated(), &timeout, nullptr)))) { //NB: this appends to waitlist
    elapsed = eventTime.roll(); //time since last wait, possibly large the first time after app launch.
    reactionTime.stop();
    auto waitedFor = reactionTime.elapsed();
    if (waitedFor < timeout) {
      ++shortwait;
    }
    waitlist.skip(numEvents);
    processList();
    return true;
  } else {
    dbg("doEvents: %s", errorText());
    return false;
  }
}

void EpollerCore::registerWith(EpollerCore &myWatcher) {
  myWatcher.watch(epfd,EPOLLIN | EPOLLRDHUP, *this);
}

//todo:1 does the dbg() have a stacking 'don't do newlines' facility?
#define epollexplain(evname) if(epevs & evname){ explainTo( "\t" #evname);}

void EpollerCore::explain(unsigned epevs, Logger &explainTo) {
  auto namedButNotReferenced = explainTo.beMerging();
  epollexplain(EPOLLIN);
  epollexplain(EPOLLPRI);
  epollexplain(EPOLLOUT);
  epollexplain(EPOLLRDNORM);
  epollexplain(EPOLLRDBAND);
  epollexplain(EPOLLWRNORM);
  epollexplain(EPOLLWRBAND);
  epollexplain(EPOLLMSG);
  epollexplain(EPOLLERR);
  epollexplain(EPOLLHUP);
  epollexplain(EPOLLRDHUP);
  epollexplain(EPOLLWAKEUP);
  epollexplain(EPOLLONESHOT);
  epollexplain(EPOLLET);
}
