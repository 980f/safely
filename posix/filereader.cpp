#include "filereader.h"



void FileReader::launch(){
  cb.aio_fildes=fd.asInt();
  // data sink
  cb.aio_buf=&buf.peek();
  // maximum to read
  cb.aio_nbytes=buf.freespace();

  //start read
  fd.errornumber= aio_read(&cb);
}

bool FileReader::operator()(){

  EraseThing(cb);//forget prior operation

  /* define the signal for the aio lib to send when something happens */
  cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  cb.aio_sigevent.sigev_signo = SIGIO;
  cb.aio_sigevent.sigev_value.sival_ptr = this;

  /* Map the Signal to the Signal Handler */
  struct sigaction sig_act;
  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO;//to get our 'this' sent back to us
  sig_act.sa_sigaction = sighandler;
  fd.errornumber = sigaction( SIGIO, &sig_act, NULL );

//didn't get the struct members that this e.g. code from ibm references, so we stick with signals for now.
//  /* Link the AIO request with a thread callback */
//   cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
//   cb.aio_sigevent.notify_function = sighandler;
//   cb.aio_sigevent.notify_attributes = NULL;
//   cb.aio_sigevent.sigev_value.sival_ptr = this;


  // file transfer args:
  // which file, if not already open will error out
  launch();
  return fd.errornumber ==0;//todo: find function on fd
}

//untested:
void FileReader::sighandler( int signo, siginfo_t *info, void */*ucontext_t context*/ )  {
  /* Ensure it's our signal */
  if (signo == SIGIO) {
    FileReader *req= reinterpret_cast<FileReader*>(info->si_value.sival_ptr);
    req->notified(info->si_code,info->si_errno);
  }
}

void FileReader::notified(int code,int errno){
  //a possibly interesting fact:
  if(code==SI_ASYNCIO){
    /* Did the request complete normally? */
    if (aio_error( &cb ) == 0) { /* Request completed successfully, get the return status of the underlying read operation */
      size_t ret = aio_return( &cb );
      if(continuation(ret)){
        //caller is responsible for rewinding the buffer
        launch();
        if(fd.errornumber!=0){
          continuation(-fd.errornumber);
        }
      }
    } else {
      continuation(-fd.errornumber);
    }
  } else {
    continuation(errno);//todo: verify this is always negative
  }

  /* Did the request complete normally? */
  if (aio_error( &cb ) == 0) { /* Request completed successfully, get the return status of the underlying read operation */
    size_t ret = aio_return( &cb );
    continuation(ret);
  } else {
    continuation(-fd.errornumber);
  }
}
