#include "fileasynchio.h"
//todo: merge with home stuff #include "nanotime.h"

FileAsyncAccess::FileAsyncAccess(bool reader, Fildes &fd, ByteScanner &buf, OnCompletion::Pointer onDone):
  amReader(reader),
  fd(fd),
  buf(buf),
  continuation(false,onDone)
{
  //#nada
}

bool FileAsyncAccess::go(){

  EraseThing(cb);//forget prior operation
  //-- no tdoing this so that we can 'cat' multple files into one buffer: buf.rewind();
  /* define the signal for the aio lib to send when something happens */
  cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  cb.aio_sigevent.sigev_signo = SIGIO;
  cb.aio_sigevent.sigev_value.sival_ptr = this;

  /* Map the Signal to the Signal Handler */
  struct sigaction sig_act;
  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO;//to get our 'this' sent back to us
  sig_act.sa_sigaction = sighandler; //NB: gnu header is wonky for this member, plays poorly with preprocessor
  return ok(sigaction( SIGIO, &sig_act, NULL )) && launch(false/* not a continuation*/);
}

void FileAsyncAccess::setHandler(OnCompletion::Pointer onDone){
  continuation=onDone;
}

bool FileAsyncAccess::block(double seconds){
  long sec=splitter(seconds);
  struct timespec ts={sec,long(1e9*seconds)};
  aiocb * list=&cb;
  return failed(aio_suspend(&list,1,&ts));
}

bool FileAsyncAccess::launch(bool more){
  if(more){
    //maydo: add 'lastSuccessfulTranferAmount' so we can loosen up retry on error in continuation
    cb.aio_offset+=cb.__return_value;//this is gnu glibc specific, could call aio_return to get the value
  } else {
    cb.aio_offset=0;
  }
  cb.aio_fildes=fd;
  if(amReader){// data sink
    cb.aio_buf=&buf.peek();
    // maximum to read
    cb.aio_nbytes=buf.freespace();
    //start read
    return ok(aio_read(&cb));
  } else {// data source
    cb.aio_buf=buf.internalBuffer();
    // maximum to send
    cb.aio_nbytes=buf.used();
    //start write
    return ok(aio_write(&cb));
  }
}

void FileAsyncAccess::sighandler( int signo, siginfo_t *info, void */*ucontext_t context*/ )  {
  if (signo == SIGIO) {
    FileAsyncAccess *req= reinterpret_cast<FileAsyncAccess*>(info->si_value.sival_ptr);
    req->notified(info->si_code,info->si_errno);
  }
}

void FileAsyncAccess::notified(int code,int ernumber){
  //a possibly interesting fact:
  if(code==SI_ASYNCIO){
    /* Did the request complete normally? */
    errornumber=ernumber;//aio_error( &cb );//unlike most posix functions this one returns what normally would go into errno
    if(isOk()){ /* Request completed successfully, get the return status of the underlying read operation */
      __ssize_t ret = aio_return( &cb );
      if(continuation(ret)){//notify recipient, return asks us to repeat
        //caller is responsible for rewinding the buffer
        if(failed(launch(true))){//if requested retry fails
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
