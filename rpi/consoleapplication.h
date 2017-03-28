#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H


#include "gpio.h"
#include "logger.h"
#include <functional>
#include "errno.h" //firstly for polling errors

#include "application.h"

#include "storedgroup.h" //base class for structured non-volatile data
#include "storednumeric.h"
#include "filer.h"
#include "storejson.h" //name is missing a 'd' :(
#include "polledtimer.h"

/** configuration classes for this project */
struct RunIndicator: public Stored {
  StoredInt whichPin;
  /** level of pin when loop is running */
  StoredBoolean onPolarity;
  RunIndicator(Storable &node);
};

struct WatchdogOptions: public Stored {
  StoredInt whichPin;
  StoredBoolean onStart;
  StoredBoolean onExit;
  WatchdogOptions(Storable &node);
};

struct Options: public Stored {
  /** example for adding another option value:*/
  StoredInt exampleInteger;
  /** watchdog configuration */
  WatchdogOptions watchdog;
  /** Run indicator*/
  RunIndicator runLamp;

  Options(Storable &node);
};



#define OwnHandler(memberfnname) std::bind(&memberfnname,this,std::placeholders::_1)

#include "polledtimer.h"
#include "chain.h"
#include "timerfd.h"

struct ConsoleApplication : public Application {
  //  out("JsonParse: after %g ms nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000, parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);
  Storable root;
  Options opts;

  TimerFD polledTimerServer;
  Chain<PolledTimer> timerService;

  /** a steady "on" while ready for events*/
  GPIO runpin;
  /** toggle now and then to indicate we are alive. */
  GPIO watchDogReset;

  Fildes hostport; // /dev/ttyS0
  Fildes tubeport; // /dev/ttyUSBx
  Fildes detectorport;//lib usb will give us the fd's we need.

  void ServePolledTimers(unsigned);

  /** record info but do nothing */
  ConsoleApplication (int argc, char *argv[]);

  /** part of startup */
  int prelims();

  /** @returns an exitcode, 0 for "no need to exit" */
  int loadOptions();

  void logOptions();
  /** output config file to console */
  void printNode(unsigned tab,Storable &node); // switch
  /** react to console input*/
  void consoleInput(unsigned events);
  /** program logic */
  int main();
};


#endif // CONSOLEAPPLICATION_H
