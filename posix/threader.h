#pragma once

#include "pthread.h"
#include "hook.h" //functional extension rather than class extension.
#include "posixwrapper.h"
#include "textkey.h"
// #include "index.h"


/** Wraps pthread. */
class Threader : public PosixWrapper {
  pthread_t handle;

  struct Attributes : PosixWrapper {
    enum Scheduler { Normal = SCHED_OTHER, Fast = SCHED_FIFO, Fair = SCHED_RR };

    Scheduler sched; //cached for debug
    sched_param param; //only used locally but kept handy for debug
    pthread_attr_t attr;

    Attributes();

    ~Attributes();

    Attributes &setDetached(bool detach);

    /** @param priority is the raw one, not niceness. check your limits (@see rlimit).*/
    Attributes &setScheduler(Scheduler sched, int priority);
  };

  /** the task will be given a reference to this, to return a completion code */
  int exitcode = 0;
  /** the task is called repeatedly with a yield between each iteration. Clear this to stop the iteration. */
  bool beRunning = false;
  //  //todo: pause
  //  Index paused;

public: //for thunker, ill advised to call directly.
  /** the thing to run */
  int routine();

public:
  /** just prep internals*/
  Threader(TextKey threadname, bool detached);

  ~Threader();

  /** set these before run()*/
  Attributes opts;
  /** set this before run(), although one could set it while running and thus change what the thread does.
   * the operand will be a reference to the exitcode. */
  Hooker<bool, int &> task;

  /** create thread and crank it up (pthread_create)*/
  bool run();

  /** create thread and crank it up (pthread_create), to execute just once. */
  bool runonce();

  /** stops thread */
  bool stop();

  // will count the pauses and match each with a proceed, and provide a RAAI class for that.
  //  bool pause();
  //  bool proceed();
  //todo:1 join
}; // class Threader
