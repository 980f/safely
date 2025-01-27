#pragma once //"(C) Andrew L. Heilveil, 2017-2018"

/** startup and eventloop */
#include "buffer.h"
#include "textpointer.h"
#include "epoller.h"
#include "posixwrapper.h"

/* a user tweaks this performance parameter to optimize event response time versus memory usage. number of open files and streams,and 2 more for some library event watchers.
 * The cost of it being too low is that you will transition from user to kernel space and back again more frequently, to get more groups of events.
 * OTOH if you are getting more than a few handfuls of events per polling attempt you probably need multiple threads with their own pollers.
 *
 *
 */
#ifndef SafelyApplicationEvents
#define SafelyApplicationEvents 9
#warning "Number of events handled per 'wait for event' has been set to a default low number, define SafelyApplicationEvents to raise it."
#endif

class Application: public PosixWrapper {
protected:
  Indexer<TextKey> arglist;
  Epoller<SafelyApplicationEvents> looper;//todo: need to bring back runtime allocation of max number of events or hae a safely build flag for this constant.

  /** time until next keepalive/sampling */
  NanoSeconds period;
  /** pid read on object creation */
  pid_t startup_pid;

private:
  /** if greater than zero and less than period it replaces period for one cycle */
  NanoSeconds quickCheck;
protected:
  /** set quickCheck if @param soonish is sooner than a prior setting */
  bool setQuickCheck(NanoSeconds soonish);

  /** clear this to try to get app to exit gracefully */
  bool beRunning;
  /** called with each event, especially when period is up. Not harmonic */
  virtual bool keepAlive();
  /** until epoll works sanely on both platforms run() will just run a timer. */
  bool justTime=true;

public:
  /** doesn't do much, but someday we may mate this to gnu getargs */
  Application(unsigned argc, char *argv[]);
  virtual ~Application()=default;
  /** show argv */
  void logArgs();
  /** show cwd */
  void logCwd();
  /** poll for and dispatch on events recorded with looper member. doesn't return under normal usage. */
  int run();
  /** quit polling */
  void stop(){
    beRunning=false;
  }
  /** "renice" */
  bool setPriority(int niceness);
  /** one tool for boosting process priority, trying to get gpio polling to 10kHz on a 3B+ */
  bool setScheduler(bool fast);
public: //utilities
  /** @returns a copy of the hostname. It is not a static function as it records errors from the attempt */
  Text hostname();
  /** write current thread's pid to file. best practice is for that file to be in /tmp so that it evaporates on a crash of the system.
   * Note: systemd knows about such files, but does not create them. It uses them to determine which child process after a fork is the 'main' process of an application. */
  static bool writepid(TextKey pidname);
};
