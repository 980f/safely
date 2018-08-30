//"(C) Andrew L. Heilveil, 2017-2018"

#include "epoller.h"

#include "sys/epoll.h"
#include "unistd.h"

#include "stdlib.h"
#include "logger.h"

#include "time.h"

//constexpr unsigned evlistsize(unsigned number){
//  return sizeof (epoll_event)*number;
//}

Epoller::Epoller(unsigned maxreport):PosixWrapper ("Epoller"),
  epfd(~0),
  BuildIndexer(epoll_event,waitlist,maxreport),
  numEvents(BadLength),//init for debug
  eventTime(true,true),//use real time, process may sleep
  elapsed(0){
  epfd=epoll_create1(0);
}

Epoller::~Epoller(){
  waitlist.destroy();//BuildIndexer uses malloc.
  close();
}

bool Epoller::close(){
  if(*this){//checking for debug purpose, avoid recording irrelevant error
    return ok(::close(epfd));
  } else {
    return true;
  }
}

bool Epoller::watch(int fd, unsigned eventbits, Handler handler){
  epoll_event event{eventbits,{&handler}};
  return ok(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event));
}

bool Epoller::modify(int fd, unsigned eventbits, Handler handler){
  epoll_event event{eventbits,{&handler}};

  return ok(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&event));
}

bool Epoller::remove(int fd){
  return ok(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr));
}

bool Epoller::wait(unsigned timeoutms){
  static StopWatch reactionTime(false,true);//perhaps using the wrong timebase caused epoller to not wait?
  static unsigned long shortwait=0;
  reactionTime.start();
  ++waitcount;
  if(okValue(numEvents, unsigned(epoll_wait(epfd,&waitlist.peek(),int(waitlist.freespace()),int(timeoutms))))){
    elapsed=eventTime.roll();//time since last wait, possibly large the first time after app launch.
    reactionTime.stop();
    double waitedFor=reactionTime.elapsed();
    if(waitedFor<timeoutms/1000.0){
      ++shortwait;
    }
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
  } else {
    ::dbg("null ptr from event returned by epoll");
  }
}

bool Epoller::doEvents(NanoSeconds timeout){
  waitlist.rewind();
  if(wait(timeout.ms())){//HEREIS the actual blocking call!
//too frequent    dbg("polled event count %d",waitlist.used());
    Indexer<epoll_event> list(waitlist,~0);
    while(list.hasNext()){
      auto event=list.next();
      //dbg("event on fd: %d",event.data.fd);
      if(event.events ==0 ){
        //we shouldn't be here.
        dbg("got a no-event response");
      } else {
        exec(event);
      }
    }
    return true;
  } else {
    dbg("doEvents: %s",errorText());
    return false;
  }
}


#define epollexplain(evname) if(epevs & evname){ dbg( #evname);}

void Epoller::explain(unsigned epevs){
  epollexplain(EPOLLIN );
  epollexplain(EPOLLPRI );
  epollexplain(EPOLLOUT );
  epollexplain(EPOLLRDNORM);
  epollexplain(EPOLLRDBAND);
  epollexplain(EPOLLWRNORM);
  epollexplain(EPOLLWRBAND);
  epollexplain(EPOLLMSG);
  epollexplain(EPOLLERR);
  epollexplain(EPOLLHUP);
  epollexplain(EPOLLRDHUP );
  epollexplain(EPOLLWAKEUP );
  epollexplain(EPOLLONESHOT);
  epollexplain(EPOLLET);
}
