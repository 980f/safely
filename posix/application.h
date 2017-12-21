#ifndef APPLICATION_H
#define APPLICATION_H "(C) Andrew L. Heilveil, 2017"

/** startup and eventloop */
#include "buffer.h"
#include "textpointer.h"
#include "epoller.h"
#include "posixwrapper.h"

class Application: public PosixWrapper {
protected:
  Indexer<TextKey> arglist;
  /** event manager */
  Epoller looper;
  /** ticks until next keepalive/sampling */
  unsigned period;
private:
  /** if greater than zero and less than period it replaces period for one cycle */
  unsigned quickCheck=0;
protected:
  /** set quickCheck if @param soonish is sooner than a prior setting */
  bool setQuickCheck(unsigned soonish);
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
  unsigned pollingTicks(double seconds);

public: //utilities
  /** @returns a copy of the hostname, not a static function as it records errors from the attempt */
  Text hostname();//not static as we record errors
  /** write pid to file. best practice is for that file to be in /tmp so that it evaporates on a crash of the system. */
  static bool writepid(TextKey pidname);
};

#endif // APPLICATION_H
