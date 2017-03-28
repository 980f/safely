#include "application.h"
#include "errno.h"

#include "logger.h"
#include "fildes.h" //todo: move writer into filer and use that

#include "fcntlflags.h"
#include "unistd.h"

#include "textpointer.h" //Text class
static Logger out("EPOLLER",true);

Application::Application(unsigned argc, char *argv[]):
  arglist(const_cast<const char **>(argv),argc*sizeof (const char *)),
  looper(32), //maydo: figure out size of maximum reasonable poll set.
  period(1), //millisecond timing, this is running on near GHz machines ...
  beRunning(false)//startup idle.
{
  out("Application Logic initialized");
}

void Application::logArgs(){
  arglist.rewind();
  while(arglist.hasNext()){
    const char *arg=arglist.next();
    out("arg[%d]=%s",arglist.ordinal()-1,arg);
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
    if(!looper.doEvents(period)){
      //some failures are not really something to get upset about
      switch (looper.errornumber) {
      case EAGAIN:
      case EINTR: //then a signal was sent, and its handler has run.
        out("nominal error ignored: %s",looper.errorText());
        break;
      case EINVAL:
        out("egregiously bad timeout or ");
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
    int howmany=fprintf(pidler,"%ld\n", long(pid));//coercing type for platform portability
    dbg("Pidfile %s should be %d bytes long",pidname, howmany);
    fflush(pidler);
    fclose(pidler);//to get it to flush asap
    return true;
  }
  return false;
}
