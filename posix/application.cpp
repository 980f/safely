#include "application.h"
#include "errno.h"

#include "logger.h"
#include "fildes.h" //todo: move writer into filer and use that

#include "fcntlflags.h"
#include "unistd.h"

static Logger mydbg("EPOLLER",true);

Application::Application(unsigned argc, char *argv[]):
  arglist(const_cast<const char **>(argv),argc*sizeof (const char *)),
  looper(32), //maydo: figure out size of maximum reasonable poll set.
  period(1), //millisecond timing, this is running on near GHz machines ...
  beRunning(false)//startup idle.
{
  mydbg("Application Logic initialized");
}

void Application::run(){
  beRunning=true;
  while(beRunning){
    if(!looper.doEvents(period)){
      //some failures are not really something to get upset about
      switch (looper.errornumber) {
      case EAGAIN:
      case EINTR: //then a signal was sent, and its handler has run.
        mydbg("nominal error ignored: %s",looper.errorText());
        break;
      case EINVAL:
        mydbg("egregiously bad timeout or ");
        stop();
        break;
      case EBADF: //looper wasn't initialized successfully
        mydbg("epoll fd was bad.");
        stop();
        break;
      }
    }
  }
}

bool Application::writepid(TextKey pidname){
  FILE* pidler(fopen(pidname,"w"));//want exclusive access
  if(pidler){
    pid_t pid=getpid();
    int howmany=fprintf(pidler,"%ld\n", long(pid));//coercing type for platform portability
    dbg("Pidfile %s should be %d bytes long",pidname, howmany);
    fflush(pidler);
    fclose(pidler);//to get it to flush asap
    return true;
  }
  return false;
}
