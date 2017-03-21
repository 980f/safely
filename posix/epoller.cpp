#include "epoller.h"

#include "sys/epoll.h"
#include "unistd.h"

#include "stdlib.h"

constexpr unsigned evlistsize(unsigned number){
  return sizeof (epoll_event)*number;
}

Epoller::Epoller(unsigned maxreport):
  epfd(~0),
  BuildIndexer(epoll_event,waitlist,maxreport)
//waitlist(reinterpret_cast<epoll_event *>(malloc(maxreport*sizeof(epoll_event))),maxreport*sizeof(epoll_event))
{
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

bool Epoller::add(int fd, epoll_event *event){
  return ok(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,event));
}

bool Epoller::modify(int fd, epoll_event *event)
{
  return ok(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,event));
}

bool Epoller::remove(int fd){
  return ok(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr));
}

bool Epoller::wait(int timeoutms){
  return ok(epoll_wait(epfd,waitlist.internalBuffer(),waitlist.allocated(),timeoutms));
}
