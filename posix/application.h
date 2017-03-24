#ifndef APPLICATION_H
#define APPLICATION_H

/** startup and eventloop */
#include "buffer.h"
#include "textkey.h"
#include "epoller.h"
#include "posixwrapper.h"

class Application: public PosixWrapper {
protected:
  Indexer<TextKey> arglist;
  Epoller looper;
  /** sampleing period in millseconds*/
  int period;
  bool beRunning;
public:
  /** doesn't do much, but someday we may mate this to gnu getargs */
  Application(unsigned argc, char *argv[]);
  /** poll for and dispatch on events recorded with looper member */
  void run();
  /** quit polling */
  void stop(){
    beRunning=false;
  }
  /** write pid to file. best practice if for that file to be in /tmp so that it evaporates on a crash of the system. */
  static bool writepid(TextKey pidname);
};

#endif // APPLICATION_H