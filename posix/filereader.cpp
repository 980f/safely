#include "filereader.h"
//todo: merge with home stuff #include "nanotime.h"

FileReader::FileReader(Fildes &fd, ByteScanner &buf, OnCompletion::Pointer onDone):
  fd(fd),
  buf(buf),
  continuation(false,onDone)
{

}

bool FileReader::go(unsigned guard){
  this->guard=guard;

  EraseThing(cb);//forget prior operation

  /* define the signal for the aio lib to send when something happens */
  cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  cb.aio_sigevent.sigev_signo = SIGIO;
  cb.aio_sigevent.sigev_value.sival_ptr = this;

  /* Map the Signal to the Signal Handler */
  struct sigaction sig_act;
  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO;//to get our 'this' sent back to us
  sig_act.sa_sigaction = sighandler; //NB: gnu header is wonky for this member, plays poorly with preprocessor
  return ok(sigaction( SIGIO, &sig_act, NULL )) && launch();
}

void FileReader::setHandler(OnCompletion::Pointer onDone){
  continuation=onDone;
}

bool FileReader::block(double seconds){
  long sec=splitter(seconds);
  struct timespec ts={sec,long(1e9*seconds)};
  aiocb * list=&cb;
  return failed(aio_suspend(&list,1,&ts));
}

bool FileReader::launch(){
  cb.aio_fildes=fd.asInt();
  // data sink
  cb.aio_buf=&buf.peek();
  // maximum to read
  cb.aio_nbytes=buf.freespace()-guard;

  //start read
  return ok(aio_read(&cb));
}

//untested:
void FileReader::sighandler( int signo, siginfo_t *info, void */*ucontext_t context*/ )  {
  /* Ensure it's our signal */
  if (signo == SIGIO) {
    FileReader *req= reinterpret_cast<FileReader*>(info->si_value.sival_ptr);
    req->notified(info->si_code,info->si_errno);
  }
}

void FileReader::notified(int code,int ernumber){
  //a possibly interesting fact:
  if(code==SI_ASYNCIO){
    /* Did the request complete normally? */
    int aern=aio_error( &cb );
    if(aern!=ernumber){
      printf("which is the real error? %d or %d",aern,ernumber);
    }
    if(ok(aern)){ /* Request completed successfully, get the return status of the underlying read operation */
      __ssize_t ret = aio_return( &cb );
      if(continuation(ret)){//notify recipient, return asks us to repeat
        //caller is responsible for rewinding the buffer
        if(failed(launch())){//if requested retry fails
          continuation(-errornumber);
          //but do not allow for chaining after a failure due to possibility of infinite loop
        }
      }
    } else {
      continuation(-errornumber);
      //but do not allow for chaining after a failure
    }
  } else {
    //wtf("Unexpected signal in " __FILE__);
  }

}
