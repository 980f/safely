#include "application.h"
#include "errno.h"

#include "logger.h"
#include "fildes.h" //todo: move writer into filer and use that

#include "fcntlflags.h"
#include "unistd.h"

#include "textpointer.h" //Text class
static Logger out("EPOLLER",true);

bool Application::setQuickCheck(unsigned soonish){
  if(soonish<quickCheck){
    quickCheck=soonish;
    return true;
  } else {
    return false;//user might want to try to set it again later.
  }
}

void Application::keepAlive(){
  //do nothing, normally overriden with code that looks for stalled processes.
}

Application::Application(unsigned argc, char *argv[]):PosixWrapper ("APP"),
  arglist(const_cast<const char **>(argv),argc*sizeof (const char *)),
  looper(32), //maydo: figure out size of maximum reasonable poll set.
  period(100), //millisecond timing, this is running on near GHz machines ...
  beRunning(false)//startup idle.
{
  out("Application Logic initialized");
}

void Application::logArgs(){
  Indexer<TextKey> listlist(arglist);
  while(listlist.hasNext()){
    const char *arg=listlist.next();
    out("arg[%d]=%s",listlist.ordinal()-1,arg);
  }

}

void Application::logCwd(){
//we use Text class because it will free what getcwd allocated. Not so critical unless we are using this program to look for memory leaks in the functions it tests.
  Text cwd(getcwd(nullptr,0));
  out("Working directory is: %s",cwd.c_str());
}

int Application::run(){
  beRunning=true;
  while(beRunning){
    //todo: libusb sometimes wants us to get back to it perhaps sooner than our period is set for.
    int nextPeriod=period;
    if(quickCheck>0){
      if(quickCheck<period){
        nextPeriod=quickCheck;
        quickCheck=0;
      } else {
        quickCheck-=period;
      }
    }
    if(looper.doEvents(nextPeriod)){
      keepAlive();
    } else {
      //some failures are not really something to get upset about
      switch (looper.errornumber) {
      case EAGAIN:
      case EINTR: //then a signal was sent, and its handler has run.
        out("nominal error ignored: %s",looper.errorText());
        break;
      case EINVAL:
        out("egregiously bad timeout or ??? ");
        stop();
        break;
      case EBADF: //looper wasn't initialized successfully
        out("epoll fd was bad.");
        stop();
        break;
      }
    }
  }
  return looper.errornumber;
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
  FILE* pidler(fopen(pidname,"w"));//want exclusive access
  if(pidler){
    pid_t pid=getpid();
    fprintf(pidler,"%ld\n", long(pid));//coercing type for platform portability
    fflush(pidler);
    fclose(pidler);//to get it to flush asap
    return true;
  }
  return false;
}
