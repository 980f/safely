#include "gpio.h"
#include "logger.h"
#include <functional>
#include "epoller.h"
#include "errno.h" //firstly for polling errors

#include "application.h"

static Logger out("MAIN");


#include "storedgroup.h" //base class for structured non-volatile data
#include "storednumeric.h"
#include "filer.h"
#include "storejson.h" //name is missing a 'd' :(

#include "polledtimer.h"


struct RunIndicator: public Stored {
  StoredInt whichPin;
  /** level of pin when loop is running */
  StoredBoolean onPolarity;
  RunIndicator(Storable &node): Stored(node),
    ConnectChild(whichPin,2), //todo: SANE CHOICE
    ConnectChild(onPolarity,0){

  }
};

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
  /** Run indicator*/
  RunIndicator runLamp;

  Options(Storable &node):Stored(node),
    ConnectChild(exampleInteger,42),
    ConnectChild(watchdog),
    ConnectChild(runLamp)
  {

  }
};

#define epollexplain(evname) if(epevs & evname){\
  out( #evname);\
}

void explain(unsigned epevs){
  epollexplain(EPOLLIN );
  epollexplain(EPOLLPRI );
  epollexplain(EPOLLOUT );
  epollexplain(EPOLLRDNORM);
  epollexplain(EPOLLRDBAND);
  epollexplain(EPOLLWRNORM);
  epollexplain(EPOLLWRBAND);
  epollexplain(EPOLLMSG);
  epollexplain(EPOLLERR);
  epollexplain(EPOLLHUP);
  epollexplain(EPOLLRDHUP );
  epollexplain(EPOLLWAKEUP );
  epollexplain(EPOLLONESHOT);
  epollexplain(EPOLLET);

}


#define OwnHandler(memberfnname) std::bind(&memberfnname,this,std::placeholders::_1)

void stupid(unsigned events){
  out("Console Events:%u",events);
  explain(events);
}

#include "polledtimer.h"
#include "chain.h"
#include "timerfd.h"

struct ConsoleApplication : public Application {
  //  out("JsonParse: after %g ms nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000, parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);
  Storable root;
  Options opts;

  TimerFD polledTimerServer;

  Chain<PolledTimer> timerService;
  GPIO runpin;
  GPIO watchDogReset;

  void ServePolledTimers(unsigned){
    for(int ti=timerService.quantity();ti-->0;){
      timerService[ti]->check();
    }
  }

  ConsoleApplication (int argc, char *argv[]):
    Application(argc,argv),
    opts(root)
  {

  }

  int prelims(){
    logArgs();
    logCwd();
    int error=loadOptions();
    if(error){
      return error;
    }

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
    return 0;
  }

  /** @returns an exitcode, 0 for "no need to exit" */
  int loadOptions(){
    Filer optionFile;
    if(! optionFile.openFile("options.json")){
      out("Couldn't open \"options.json\", error:[%d]%s",optionFile.errornumber,optionFile.errorText());
      return optionFile.errornumber;
    }
    if(! optionFile.readall(~0U)){
      out("Couldn't open \"options.json\", error:[%d]%s",optionFile.errornumber,optionFile.errorText());
      return optionFile.errornumber;
    }
    //free up options file for external editing (in case we ever open_exclusive)
    optionFile.close();//this does not lose the data already read.
    ByteScanner optsText(optionFile.contents());
    Indexer<const char> arg(reinterpret_cast<const char *>(optsText.internalBuffer()),optsText.allocated());//type casting

    StoreJsonParser parser(arg);
    parser.core.root=&root;
    parser.parse();
    //could print some stats here:    parser.stats.maxDepth
    return 0;
  }

  void logOptions(){
    out("Options:");
    printNode(0,root);
  }

  //todo: rework to use logger. Need logger to support 'disable auto newline'
  void printNode(unsigned tab,Storable &node){
    bool pretty=Index(tab).isValid();
    if(pretty){
      putchar('\n');
      for(unsigned tabs = tab; tabs-->0; ) {
        printf("  ");
      }
    }
    if(node.name.empty()) {
      //just print tabs
    } else {
      printf("\"%s\" : ",node.name.c_str());
    }
    switch (node.getType()) {
    case Storable::Wad:
      printf("{");
      for(auto list(node.kinder()); list.hasNext(); ) {
        Storable & it(list.next());
        printNode(pretty?tab + 1:BadIndex,it);
        if(list.hasNext()) {
          putchar(',');
        } else {
          if(pretty){
            putchar('\n');
            for(unsigned tabs = tab; tabs-->0; ) {
              printf("  ");
            }
          }
          putchar('}');
        }
      }
      break;
    case Storable::Numerical:
      printf("%g ",node.getNumber<double>());
      break;
    case Storable::Uncertain:
    case Storable::NotKnown:
      printf("%s ",node.image().c_str());
      break;
    case Storable::Textual:
      if(node.image().empty()){
        putchar('"');
        //else printf converts null ptr to (null)
        putchar('"');
      } else {
        printf("\"%s\" ",node.image().c_str());
      }
      break;
    } // switch
    fflush(stdout);
  } // switch

  void consoleInput(unsigned events){
    out("Console Events:%u",events);    
  }

  int main(){
    if(int ercode=prelims()){
      return ercode;
    }

    runpin.connectTo(opts.runLamp.whichPin);
    runpin.configure(1);
    runpin=opts.runLamp.onPolarity;

    watchDogReset.connectTo(opts.watchdog.whichPin);
    watchDogReset.configure(1);//simple output

    watchDogReset=opts.watchdog.onStart;
    writepid("pidfile");//in cwd while we are creating program, will be in /tmp when in production.

    //setup event handlers.
    looper.watch(polledTimerServer.asInt(), EPOLLIN | EPOLLET, OwnHandler(ConsoleApplication::ServePolledTimers));
//    auto step2=         std::bind(&ConsoleApplication::consoleInput,this,std::placeholders::_1);
    looper.watch(0,EPOLLRDNORM, OwnHandler(ConsoleApplication::consoleInput));


    //process events
    int exitcode=Application::run();//all activity from this point in is via callbacks arranged in the previous few lines.
    runpin= ! opts.runLamp.onPolarity;
    watchDogReset=opts.watchdog.onExit;
    return exitcode;
  }
};

int main(int argc, char *argv[]){
  ConsoleApplication app(argc,argv);
  int exitcode=app.main();
  out("Program exit code:%d",exitcode);
  return exitcode;
}
