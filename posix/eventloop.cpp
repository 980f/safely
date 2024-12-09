
#include "eventloop.h"
#include "poll.h"

EventLoop::EventLoop():poller(32){

}

void EventLoop::dispatch(){
  unsigned coindex=0;
  for(auto &ev:fdlist){
    if(ev.revents>0){
      auto aHandler=handlers.at (coindex);
      aHandler();
      ev.revents=0;//will later embellish handlers
    }
    ++coindex;//todo: can we get loop index of a foreach?
  }
}

void EventLoop::run(){
  beRunning=true;
  while(beRunning){//will be true until one of the event handlers calls cancel().
    int numset=poll (fdlist.data (),fdused,nextTimeout);
    if(numset<0){
      //there were problems ...
    }
    dispatch();
  }
}

void EventLoop::cancel(){
  beRunning=false;
}

int EventLoop::addInputWatch(int fd, EventLoop::Handler watcher){
  //find free entry

}

#if 0
for long running procedures we wish to do a form of yielding,
a yield involves returning from an event handler while scheduling an event posting that will happen
after all pending at that time events have been handled.
That entails do a poll() call with immediate return adding the returned notificiations to the list.
So, we need to see if our poll() implementation clears revents or not.

We can loop through the list of event counts and do a poll() every now and then until we get no events and have none still in our list
at which time we then do a poll with sleep.

If launching a seperate thread makes sense then doing so with fork and registering events on the fork's return fd's allows notification based cooperation.


#endif
