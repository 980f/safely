#include "threader.h"

/** placeholder method, this thread does nothing if you don't overload this method. */
int Threader::routine(){
  while(beRunning) {
    //someone clearing beRunning is a definitive end to this loop, as is the body returning false;
    beRunning &= task(exitcode);
    beRunning &= !failure(pthread_yield());
  }
  return exitcode;//presumably set by runbody on a soft exit.
}

Threader::Threader(TextKey threadname, bool detached) : PosixWrapper(threadname),
  task(false){//default task is to exit
  opts.setDetached(detached);
}

Threader::~Threader(){
  //todo: cancel etc and try really hard to stop the thread.

}

void *threadThunk(void *threader){
  if(threader) {
    Threader &thread(*reinterpret_cast<Threader *>(threader));
    thread.routine();
  }
  return nullptr;
}

bool Threader::run(){
  beRunning = true;//to make sure it is set in case the create runs the new thread before returning
  if(failure(pthread_create(&handle,&(opts.attr),&threadThunk,this))) {
    beRunning = false;
    return false;
  } else {
    return true;// we started, although the thread might complete before we return.
  }
}

bool Threader::runonce(){
  if(failure(pthread_create(&handle,&(opts.attr),&threadThunk,this))) {
    beRunning = false;
    return false;
  } else {
    return true;// we started, although the thread might complete before we return.
  }

}

bool Threader::stop(){
  beRunning = false;
  return !failure(pthread_cancel(handle));
}

//bool Threader::runbody(){
//  failure(pthread_yield());
//  return true;
//}

Threader::Attributes::Attributes() : PosixWrapper("Threader::Atrributes"){
  failure(pthread_attr_init(&attr));
}

Threader::Attributes::~Attributes(){
  failure(pthread_attr_destroy(&attr));
}

Threader::Attributes &Threader::Attributes::setDetached(bool detach){
  pthread_attr_setdetachstate(&attr,detach ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
  return *this;
}

Threader::Attributes &Threader::Attributes::setScheduler(Threader::Attributes::Scheduler sched, int priority){
  if(!failure(pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED))) {
    if(!failure(pthread_attr_setschedpolicy(&attr, sched))) {
      this->sched = sched;
    }
    param.sched_priority = priority;
    failure(pthread_attr_setschedparam(&attr,&param));
  }
  return *this;
}
