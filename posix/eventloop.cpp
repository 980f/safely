#include "eventloop.h"
#include "posixwrapper.h"
#include "fildes.h"
#include "poll.h"
#include "nanoseconds.h"
#include "eztypes.h" //for u64
///////////////////
#include <sys/timerfd.h>

/** wrap Timer file descriptors for use with eventloop polling.
 * We force some usage internally here, we don't expose 100% timer fd functionality.
*/
class TimerFd: public Fildes {
  int clockid;//cached timer selection

  int flags; //cached flag settings, for debug
public:

  enum Flag {
    absolute=TFD_TIMER_ABSTIME,//no documentation
    cloexec=TFD_CLOEXEC,  //"close on execute" seems to only matter if you will be exec'ing, to keep the object private to the creating thread.
    nonblock=TFD_NONBLOCK, //we'll probably force usage of this
    //there are other not particularly useful bits
  };

  TimerFd(bool realworld, int flags):
    clockid(realworld?CLOCK_MONOTONIC : CLOCK_THREAD_CPUTIME_ID ),
    flags(flags){
    preopened(timerfd_create(clockid, flags|nonblock),true/* we are owner*/);
  }

  int setTime(double seconds,bool oneshot, double *priorseconds){
    itimerspec new_value;
    parseTime(new_value.it_value,seconds);//first delay
    if(oneshot){
      new_value.it_interval={0,0};
    } else {
      new_value.it_interval=new_value.it_value; //periodic delay
    }

    itimerspec prev_value;
    int errc=timerfd_settime(fd, flags, &new_value, &prev_value);
    if(errc==0){
      if(priorseconds!=nullptr){
        *priorseconds=from(prev_value.it_value);
      }
    }
    return errc;
  }
  //int timerfd_gettime(int fd, struct itimerspec *curr_value);

  //number of rollovers since timer was first created
  u64 rollovers(){
    u64 buffer=0;
    ByteScanner bs(reinterpret_cast<u8 *>(&buffer),sizeof(buffer));
    if(read(bs)==8){
      return buffer;
    } else {
      return 0;
    }
  }
};


///////////////////
/// and now for the main attraction:
///
EventLoop::EventLoop(){

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
