#include "epoller.h"

#include "sys/epoll.h"
#include "unistd.h"

#include "stdlib.h"

constexpr unsigned evlistsize(unsigned number){
  return sizeof (epoll_event)*number;
}

Epoller::Epoller(unsigned maxreport):
  epfd(~0),
  BuildIndexer(epoll_event,waitlist,maxreport){
  epfd=epoll_create1(0);
}

Epoller::~Epoller(){
  waitlist.destroy();
  close();
}

bool Epoller::close(){
  if(*this){//checking for debug purpose, avoid recording irrelevant error
    return ok(::close(epfd));
  } else {
    return true;
  }
}

bool Epoller::watch(int fd, unsigned eventbits, Handler &handler){
  epoll_event event{eventbits,{&handler}};
  return ok(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event));
}

bool Epoller::modify(int fd, unsigned eventbits,Handler &handler){
  epoll_event event{eventbits,{&handler}};

  return ok(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&event));
}

bool Epoller::remove(int fd){
  return ok(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr));
}

bool Epoller::wait(int timeoutms){
  if(okValue(numEvents, epoll_wait(epfd,&waitlist.peek(),waitlist.freespace(),timeoutms))){
    waitlist.skip(numEvents);
    return true;
  } else {
    return false;
  }
}

void Epoller::exec(const epoll_event &ev){
  if(ev.data.ptr){
    Handler &handler(*reinterpret_cast<Handler *>(ev.data.ptr));
    handler(ev.events);
  }
}

bool Epoller::doEvents(int timeoutms){
  waitlist.rewind();
  if(wait(timeoutms)){//HEREIS the actual blocking call!
    Indexer<epoll_event> list(waitlist,~0);
    while(list.hasNext()){
      auto event=list.next();
      if(event.events ==0 ){
        //we shouldn't be here.
        logmsg("got a no-event response");
      } else {
        exec(event);
      }
    }
    return true;
  } else {
    logmsg("doEvents: %s",errorText());
    return false;
  }
}
