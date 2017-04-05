#ifndef THREADER_H
#define THREADER_H

#include "pthread.h"

#include "posixwrapper.h"

class Threader: public PosixWrapper {
  pthread_t handle;
  struct Attributes:public PosixWrapper {
    pthread_attr_t attr;
    Attributes();
    ~Attributes();
    Attributes &setDetached(bool detach);
  } opts;
  int exitcode;
  bool beRunning;

public://for thunker, ill advised to call directly.
  /** the thing to run */
  virtual int routine();

public:
  /** just prep internals*/
  Threader(bool detached);
  virtual ~Threader();

  /** create thread and crank it up (pthread_create)*/
  bool run();

  //todo: join, but today I just want a reader loop.

  virtual bool runbody();
};

#endif // THREADER_H
