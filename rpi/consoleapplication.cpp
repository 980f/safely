#include "consoleapplication.h"
#include "errno.h" //firstly for polling errors
#include "filer.h"
#include "storejson.h" //name is missing a 'd' :(
#include "filename.h"

//needing to get on with development, can't get stable thunking so ...
static ConsoleApplication *me(nullptr);

//void consoleThunk(unsigned evs){
//  if(me) me->consoleInput(evs);
//}

//void timerThunk(unsigned evs){
//  if(me) me->ServePolledTimers(evs);
//}


#define EventHandler(memberfnname) [](unsigned evs){if(me) me->memberfnname(evs);}


RunIndicator::RunIndicator(Storable &node): Stored(node),
  ConnectChild(whichPin,2), //todo: SANE CHOICE
  ConnectChild(onPolarity,0){
  //#nada
}

WatchdogOptions::WatchdogOptions(Storable &node): Stored(node),
  ConnectChild(whichPin,21), //pin 21 is on the end of the connector, easy to access.
  ConnectChild(onStart,0),
  ConnectChild(onExit,0){
  //#nada
}

Options::Options(Storable &node):Stored(node),
  ConnectChild(exampleInteger,42),
  ConnectChild(watchdog),
  ConnectChild(runLamp),
  ConnectChild(hostport),
  ConnectChild(tubeport)
{
  //#nada
}

void ConsoleApplication::ServePolledTimers(unsigned){
  for(int ti=timerService.quantity();ti-->0;){
    timerService[ti]->check();
  }
}

ConsoleApplication::ConsoleApplication(int argc, char *argv[]):
  Application(argc,argv),
  opts(root)
{
  me=this;//feed ourselves to the thunker.
}

int ConsoleApplication::prelims(){
  logArgs();
  logCwd();
  int error=loadOptions();
  if(error){
    return error;
  }

  if(! MemoryMapper::init(true)){
    logmsg("failed to acquire GPIO access, MemoryMapper::init failed. \nThis depends upon a link named \"safe-memory\", pointing to either /dev/gpiomem or a file big enough to simulate the RPI iospace");
    logmsg("dd bs=160 count=1 if=/dev/zero of=dummy-memory");
    logmsg("");
    logmsg("If you get a SIGBUS then you may not have rights to run this program");
    if(MemoryMapper::Mmap){
      return MemoryMapper::Mmap->getError();
    } else {
      logmsg("couldn't allocate a memory mapper");
      return ENOMEM;
    }
  }

  console.preopened(0,false);//wrap standard console fd
  //try all connections before bailing out on bad ones
  hostport.connect(opts.hostport);
  tubeport.connect(opts.tubeport);
  //connect dp5
  if(hostport.fd.isOpen() && tubeport.fd.isOpen()){
    return 0;
  } else {
    logmsg("quiting because not all ports opened. Fix hardware and try again");
    return EBADFD;
  }
}

int ConsoleApplication::loadOptions(){
  Filer optionFile;
  Text hostname=this->hostname();
  DottedName optsname('.',hostname);
  optsname.parse("json");
  if(! optionFile.openFile(optsname.pack(FileNameConverter()))){
    logmsg("Couldn't open \"options.json\", error:[%d]%s",optionFile.errornumber,optionFile.errorText());
    return optionFile.errornumber;
  }
  if(! optionFile.readall(~0U)){
    logmsg("Couldn't open \"options.json\", error:[%d]%s",optionFile.errornumber,optionFile.errorText());
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

void ConsoleApplication::logOptions(){
  logmsg("Options:");
  printNode(0,root);
}

void ConsoleApplication::printNode(unsigned tab, Storable &node){
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
}

void ConsoleApplication::consoleInput(unsigned events){
  logmsg("Console Events:%u",events);
  if(events&EPOLLRDNORM){
    ByteScanner castit(command.receiver);
    if(console.read(castit)>0){//if something was actually read
      if(command.commandPresent()){
        logmsg("Console command: %s",command.receiver.internalBuffer());
      }
    }
  }
}

void ConsoleApplication::hostInput(unsigned events){
  logmsg("Host Events:%u",events);
}

void ConsoleApplication::tubeResponse(unsigned events){
  logmsg("Tube Events:%u",events);
}

int ConsoleApplication::main(){
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
  looper.watch(polledTimerServer.asInt(), EPOLLIN | EPOLLET, EventHandler(ServePolledTimers));
  looper.watch(0,EPOLLRDNORM, EventHandler(consoleInput));
  looper.watch(hostport.fd.asInt(),EPOLLIN,EventHandler(hostInput));
  looper.watch(tubeport.fd.asInt(),EPOLLIN,EventHandler(tubeResponse));
  //process events
  int exitcode=Application::run();//all activity from this point in is via callbacks arranged in the previous few lines.
  runpin= ! opts.runLamp.onPolarity;
  watchDogReset=opts.watchdog.onExit;
  return exitcode;
}
