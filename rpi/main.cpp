#include "gpio.h"
#include "logger.h"
//#include "unistd.h"
#include "epoller.h"
#include "errno.h" //firstly for polling errors

#include "application.h"

static Logger out("MAIN");


#include "storedgroup.h" //base class for structured non-volatile data
#include "storednumeric.h"
#include "filer.h"
#include "storejson.h" //name is missing a 'd' :(

struct WatchdogOptions: public Stored {
  StoredInt whichPin;
  StoredBoolean onStart;

  StoredBoolean onExit;
  WatchdogOptions(Storable &node): Stored(node),
    ConnectChild(whichPin,21), //pin 21 is on the end of the connector, easy to access.
    ConnectChild(onStart,0),
    ConnectChild(onExit,0)
  {

  }


};

struct Options: public Stored {
  /** example for adding another option value:*/
  StoredInt exampleInteger;
  /** watchdog configuration */
  WatchdogOptions watchdog;
  Options(Storable &node):Stored(node),
    ConnectChild(exampleInteger,42),
    ConnectChild(watchdog){

  }
};


int main(int argc, char *argv[]){
  Application app(argc,argv);
  app.logArgs();
  app.logCwd();

  Filer optionFile;
  if(! optionFile.openFile("options.json")){
    out("Couldn't open \"options.json\", error:[%d]%s",optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  if(! optionFile.readall(~0U)){
    out("Couldn't open \"options.json\", error:[%d]%s",optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  //free up options file for external editing (without confusion)
  optionFile.close();//this does not lose the data already read.
  ByteScanner optsText(optionFile.contents());
  Indexer<const char> arg(reinterpret_cast<const char *>(optsText.internalBuffer()),optsText.allocated());//type casting

  StoreJsonParser parser(arg);
//  StopWatch perftimer;
  parser.parse();

  if(! parser.core.root){
    out("failed to read options data, proceeding with builtin defaults");
    parser.core.root=new Storable();//dummy root
  }
//  out("JsonParse: after %g ms nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000, parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);
  Options opts(*parser.core.root);


  if(! MemoryMapper::init(true)){
    out("failed to acquire GPIO access, MemoryMapper::init failed. \nThis depends upon a link named \"safe-memory\", pointing to either /dev/gpiomem or a file big enough to simulate the RPI iospace");
    out("dd bs=160 count=1 if=/dev/zero of=dummy-memory");
    out("");
    out("If you get a SIGBUS then you may not have rights to run this program");
    if(MemoryMapper::Mmap){
      return MemoryMapper::Mmap->getError();
    } else {
      out("couldn't allocate a memory mapper");
      return ENOMEM;
    }
  }

  GPIO watchDogReset(opts.watchdog.whichPin);

  watchDogReset.configure(1,0);//simple output
  watchDogReset=1;
  app.writepid("pidfile");//in cwd while we are creating program, will be in /tmp when in production.


  int exitcode=app.run();
  //todo: configure "watchDogReset level on exit"
  return exitcode;
}
