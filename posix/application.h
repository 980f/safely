#ifndef APPLICATION_H
#define APPLICATION_H

/** startup and eventloop */
#include "buffer.h"
#include "textpointer.h"
#include "epoller.h"
#include "posixwrapper.h"

class Application: public PosixWrapper {
protected:
  Indexer<TextKey> arglist;
  Epoller looper;
  /** sampling period in millseconds*/
  unsigned period;
  /** if greater than zero and less than period it replaces period for one cycle */
private:
  unsigned quickCheck=0;
protected:
  bool setQuickCheck(unsigned soonish);
  /** clear this to try to get app to exit gracefully */
  bool beRunning;
  /** called with each event, especially when period is up. Not harmonic */
  virtual bool keepAlive();

  //until epoll works sanely on both platforms run() will just run a timer.
  bool justTime;

public:
  /** doesn't do much, but someday we may mate this to gnu getargs */
  Application(unsigned argc, char *argv[]);
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

public: //utilities
  Text hostname();//not static as we record errors
  /** write pid to file. best practice if for that file to be in /tmp so that it evaporates on a crash of the system. */
  static bool writepid(TextKey pidname);
};

#endif // APPLICATION_H
