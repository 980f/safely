#pragma once //"(C) Andrew L. Heilveil, 2017-2018"

/** startup and eventloop */
#include "buffer.h"
#include "textpointer.h"
#include "epoller.h"
#include "posixwrapper.h"

class Application: public PosixWrapper {
protected:
  Indexer<TextKey> arglist;
  Epoller looper;

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
