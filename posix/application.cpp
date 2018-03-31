
//"(C) Andrew L. Heilveil, 2017-2018"

#include "application.h"
#include "errno.h"

#include "logger.h"
#include "fildes.h" //todo: move writer into filer and use that

#include "fcntlflags.h"
#include "unistd.h"
#include "sys/resource.h"  //setPriority
#include "sched.h" //setSched
#include "textpointer.h" //Text class

#include "time.h" //for epoll override
#include "nanoseconds.h"
#include "cheaptricks.h" //take()


bool Application::setQuickCheck(NanoSeconds soonish){
  if(soonish.inFuture()){
    quickCheck.atMost(soonish);

    return true;
  } else {
    return false;//user might want to try to set it again later.
  }
}

bool Application::keepAlive(){
  //do nothing, normally overriden with code that looks for stalled processes.
  return true;
}


Application::Application(unsigned argc, char *argv[]):PosixWrapper ("APP"),//todo:1 name from arg0 last member
  arglist(const_cast<const char **>(argv),argc*sizeof (const char *)),
  hz(1000.0), //start with epoll's value
  looper(32), //maydo: figure out size of maximum reasonable poll set.
  period(NanoSeconds(0.1)), //start at 10 Hz, a rather slow value.
  beRunning(false){//startup idle.
  startup_pid = getpid();
  dbg("Application base initialized, pid:%lu",static_cast<unsigned long>(startup_pid));//coerce type cause printf is dumb
}

void Application::logArgs(){
  Indexer<TextKey> listlist(arglist);
  while(listlist.hasNext()){
    const char *arg=listlist.next();
    dbg("arg[%d]=%s",listlist.ordinal()-1,arg);
  }
}

void Application::logCwd(){
//we use Text class because it will free what getcwd allocated. Not so critical unless we are using this program to look for memory leaks in the functions it tests.
  Text cwd(getcwd(nullptr,0));
  dbg("Working directory is: %s",cwd.c_str());
}

int Application::run(){
  beRunning=true;

  while(beRunning){//this is what some people call "the event loop"
    NanoSeconds nextPeriod=period;
    //first use: libusb sometimes wants us to get back to it perhaps sooner than our period is set for.
    if(quickCheck.signabs()>0){

      if(quickCheck<period){
        nextPeriod=take(quickCheck);
      } else {
        quickCheck-=period;
      }
    }
    if(justTime){ //added to deal with corruption of callbacks on raspberry pi, ignore callbacks.
      while(nextPeriod.sleep());//returns 0 on normal completion, else errno is set and dregs is timeremaining
      looper.elapsed=looper.eventTime.roll();//emulate looper's wait.
      beRunning=keepAlive();
    } else {
      if(looper.doEvents(nextPeriod)){
        beRunning=keepAlive();
      } else {
        //some failures are not really something to get upset about
        switch (looper.errornumber) {
        case EAGAIN:
        case EINTR: //then a signal was sent, and its handler has run.
          dbg("nominal error ignored: %s",looper.errorText());
          break;
        case EINVAL:
          dbg("egregiously bad timeout or ??? ");
          stop();
          break;
        case EBADF: //looper wasn't initialized successfully
          dbg("epoll fd was bad.");
          stop();
          break;
        }
      }
    }
  }
  return looper.errornumber;
}

bool Application::setPriority(int niceness){
  if(niceness<-19){
    return setScheduler(true);
  }
  if(ok(setpriority(PRIO_PGRP,0,niceness))){//the middle 0 is "us".
    return true;
  } else {
    dbg("Failed to setpriority to %d",niceness);
    return false;
  }
}

bool Application::setScheduler(bool fast){
  int policy=fast?SCHED_FIFO:SCHED_OTHER;
  sched_param param;
  param.sched_priority=95;//number cadged from audio
  if(ok(sched_setscheduler(startup_pid,policy,&param))){
    return true;
  } else {
    dbg("Failed to setscheduler to %d",policy);
    return false;
  }
}

Text Application::hostname(){
  char maxname[512];
  if(ok(gethostname(maxname,512))){
    return Text(maxname);//copies content
  } else {
    return Text("noname");
  }
}

bool Application::writepid(TextKey pidname){
  pid_t pid=getpid();//get calling thread's pid
  ::dbg("pid: %ld\n", long(pid));//coercing type for platform portability
  FILE* pidler(fopen(pidname,"w"));//want exclusive access
  if(pidler){
    fprintf(pidler,"%ld\n", long(pid));//coercing type for platform portability
    fflush(pidler);
    fclose(pidler);//to get it to flush asap
    return true;
  }
  return false;
}
