#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include "application.h"
#include "gpio.h"
#include "serialdevice.h"
#include "dp5device.h"

#include "logger.h"

#include "polledtimer.h"
#include "chain.h"
#include "timerfd.h"

#include "command.h"

#include "storednumeric.h"

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
  SerialConfiguration hostport;
  SerialConfiguration tubeport;

  Options(Storable &node);
};



struct ConsoleApplication : public Application {
  //  out("JsonParse: after %g ms nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000, parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);
  Storable root;
  Options opts;

  /** a steady "on" while ready for events*/
  GPIO runpin;
  /** toggle now and then to indicate we are alive. */
  GPIO watchDogReset;


  TimerFD polledTimerServer;
  Chain<PolledTimer> timerService;

  Command command;
  Fildes console;
  SerialDevice hostport; // /dev/ttyS0
  SerialDevice tubeport; // /dev/ttyUSBx
  DP5Device detectorport;//lib usb will give us the fd's we need.



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
  /** react to host input */
  void hostInput(unsigned events);
  /** react to tube response*/
  void tubeResponse(unsigned events);

  /** the application entry */
  int main();
};


#endif // CONSOLEAPPLICATION_H
