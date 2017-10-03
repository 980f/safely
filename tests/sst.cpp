//"(C) Andrew L. Heilveil, 2017"

/*
test harness for Stored/Json socket protocol.

As a master:
It will create a data structure and send that to the remote.
It will edit the local data structure with incoming chunks.
It will not execute the commands such as "delete node", at least not until it shares the same code base.

When that remote is the old desktop gui we should get information as it is edited on that gui.


Special functions of the remote:
delete node by name.
"please send" by name.

*/

#include <polledtimer.h>
#include <dottedname.h>
#include "application.h" //base class for non-demon programs

#include "timerfd.h"

#include "telnetserver.h"
#include "storednumeric.h"
#include "storedlabel.h"

struct ConsoleOptions : public Stored {
  StoredBoolean enabled;
  /** service port */
  StoredCount port;
  /** tcp server parameter */
  StoredCount backlog;
  /** number of allowed connections */
  StoredCount allowed;
  /** interface to serve on, name or dotted decimal */
  StoredLabel interface;
  ConsoleOptions(Storable &node);
};


struct Options : public Stored {
  /** debug access connection options */
  ConsoleOptions console;
  /** rate at which main loop processes logic */
  StoredReal logicHz;
  Options(Storable &node);
};


/** the 'business logic' of this program */
class SST : public Application {
  /** this and that program options, things that aren't tied closely to some piece of the hardware */
  Options options;
  /** shared timer timebase */
  TimerFD timerSource;
  /** 'soft' timers, one appends derived classes of PolledTimer to this guy.*/
  Chain<PolledTimer> timerService;

//  /** debug command buffer */
//  Command consoleCommand;
  u8 incoming[32 * 1024];
  Indexer<u8> receiver;

  u8 outgoing[32 * 1024];
  Indexer<u8> xmitter;

  TelnetServer console;
  /** commands to host port have their human readable version sent here: (default wraps stdout)*/
  Fildes status;

  /** process human readable command into action request */
  void doCommand(DottedName &cmd, Fildes &replyto);
  /** process human readable command into action request */
  void doCommand(ReadonlySequence<TextKey> &cmd, Fildes &replyto);


  /** we need a state machine to shut down, and this is the result passed out of the last stage as to why we are doing so. */
  int shutdownProcess = 0;
  void exitProgram(int Ereason);

  /** until epoll behaves this is where all events are detected. Once epoll does what it is advertised to do this will be used to look for missed events. */
  bool keepAlive() override;
public:
  /** this constructor just collects configuration data, it doesn't touch the real world. */
  SST(unsigned argc, char *argv[]);
  //to get rid of 'no virtual destructor' warning.
  virtual ~SST() = default;

  /** the application entry */
  int main();

private:
  /** open ports etc.  */
  int prelims();

  /** read the configuration file
   * @returns an exitcode, 0 for "no need to exit" */
  int loadOptions();
public:
  void pollConsole(Socketeer &sock);
  void pollConsoles();
  void consoleUpdate(Socketeer &sock, Indexer<u8> &p);
  void updateConsoles(Indexer<u8> &&ref);
  /** this updates the 'soft' timers */
  void ServePolledTimers(unsigned);
  /** commandable actions */
  bool setParam(ReadonlySequence<TextKey> &it, Fildes &replyto);
  void getParam(ReadonlySequence<TextKey> &it, Fildes &replyto);

  bool quitProgram(ReadonlySequence<TextKey> &it, Fildes &replyto);

  void poll(ReadonlySequence<TextKey> &it, Fildes &replyto);
  unsigned ticksForInterval(double seconds);
private: //commands
  /** exit application, never a normal thing to do. */
  void shutdown();
  bool commandPresent(unsigned int *termloc);
};

#include "jsonfile.h"
//#include "filename.h"

#include <unistd.h> //STDIN_FILENO
#include <filenameconverter.h>
//#include "fcntlflags.h" //to open control port

///printable version
#define PRversion "Argon-"

//at end of each sending to console:
#define PRompt "\n> "

static const char signon[] = "SST console.\nVersion= " PRversion PRompt;
//for diagnostic messages that happen too often for normal operation
static unsigned spew = 0;

//epoll is failing miserably on rpi, so we do the simplest service function that we can without reworking all of the source
static SST *that = nullptr;

#if 1
//#define EventHandler(memberfnname) [this] (unsigned evs){this->memberfnname(evs);}
#else
//take advantage of singleton :(
#define EventHandler(memfun) & thunk_ ## memfun

#define thunkit(memfun) void thunk_ ## memfun(unsigned evs){ \
    that->memfun(evs); \
}

thunkit(ServePolledTimers);
thunkit(hostInput);

#endif

Options::Options(Storable &node) : Stored(node),
                                   ConnectChild(console),
                                   ConnectChild(logicHz, 100) {
  //#nada
}


struct DownShift : public ReadonlySequence<TextKey> {
  ChainScanner<Text> &wrapped;
public:
  DownShift(ChainScanner<Text> &wrapped) : wrapped(wrapped) {
  }

  // Sequence interface
public:
  bool hasNext() override {
    return wrapped.hasNext();
  }

  TextKey next() override {
    return wrapped.next();//.c_str();
  }
};

class CrossShift : public ReadonlySequence<TextKey> {
  Indexer<TextKey> &wrapped;
public:
  CrossShift(Indexer<TextKey> &wrapped) : wrapped(wrapped) {
  }

  // Sequence interface
public:
  bool hasNext() override {
    return wrapped.hasNext();
  }

  TextKey next() override {
    return wrapped.next();//.c_str();
  }
}; // class CrossShift


void SST::doCommand(DottedName &cmd, Fildes &replyto) {
  //command has been stripped of checksum and separated at the unquoted commas.
  auto it = cmd.indexer();
  DownShift arf(it);
  while (arf.hasNext()) {
    doCommand(arf, replyto);
  }
} // SST::doCommand


template<typename Numeric> Numeric getNarg(ReadonlySequence<TextKey> &it, Numeric defawlt) {
  if (it.hasNext()) {
    return Cstr(it.next()).cvt(defawlt);
  }
  return defawlt;
}

void dumpNode(Storable &node, const Cstr &info, Fildes &fd) {
  JsonFile detFile(node);
  fd.write(info.raw(), info.length());
  fd.writeChars(':', 2);//double colon, like C++ class.
  detFile.printOn(fd, 0);
  fd.writeChars('\n', 1);
}

void sendString(Fildes &fd, const Cstr &cstr) {
  fd.write(cstr.raw(), cstr.length());
}


bool SST::setParam(ReadonlySequence<TextKey> &it, Fildes &replyto) {
  if (it.hasNext()) {
    Text path = it.next();
    Storable *entity = Stored::Groot(0).findChild(path, false);
    if (entity) {
      if (it.hasNext()) {
        Text value = it.next();
        if (entity->numChildren() > 0) {
          sendString(replyto, "That is a group, it does't have a value" PRompt);
        } else {
          entity->setImageFrom(value, Storable::Quality::Parsed);
          Cstr image = entity->image();
          sendString(replyto, image);
          sendString(replyto, PRompt);
        }
      } else {
        sendString(replyto, "Follow name with value, no '=' or ';', just space " PRompt);
      }
    } else {
      sendString(replyto, "Nothing by that name, use 'get /' to see everything" PRompt);
    }
  } else {//no name
    sendString(replyto, "follow with a slashed path to the item of interest" PRompt);
  }
  return false;
} // SST::setParam

void SST::getParam(ReadonlySequence<TextKey> &it, Fildes &replyto) {
  if (it.hasNext()) {
    Text path = it.next();
    bool autocreate = false;
    bool remove = false;

    if (it.hasNext()) {
      Text extra = it.next();
      if (extra == "create") {
        autocreate = true;
      }
      if (extra == "delete") {
        remove = true;
      }
    }
    Storable *entity = Stored::Groot(0).findChild(path, autocreate);
    if (entity) {
      if (remove) {
        unsigned which = entity->ownIndex();
        entity = entity->parent;
        if (entity) {
          entity->remove(which);
        }
      } else {
        Cstr image = entity->image();
        if (entity->numChildren() > 0) {
          dumpNode(*entity, image, replyto);
        } else {
          sendString(replyto, image);
          sendString(replyto, PRompt);
        }
      }
    } else {
      sendString(replyto, "Nothing by that name, use 'get /' to see everything" PRompt);
    }
  } else {
    sendString(replyto, "follow with a slashd path to the item of interest" PRompt);
  }
} // SST::getParam


bool SST::quitProgram(ReadonlySequence<TextKey> &it, Fildes &replyto) {
  shutdownProcess = getNarg(it, 0);
  shutdown();//early response
  sendString(replyto, "Restarting program.");
  return true;//may or may not get out a response before we die.
}

void SST::loadOptionsFile(ReadonlySequence<TextKey> &it, Fildes &replyto) {
  if (it.hasNext()) {
    Text filename = it.next();
    Text loadpoint = it.hasNext() ? it.next() : "/";
    Storable *target = Stored::Groot(0).findChild(loadpoint);
    //load a different control file
    if (target) {
      if (JsonFile(*target).loadFile(filename)) {
        sendString(replyto, "Failed to load options." PRompt);
      } else {
        //StoredGroups need to see nodes added and add members to match.
      }
    } else {
      sendString(replyto, "Failed to load options , unknown node" PRompt);
    }
  } else {
    //reload default file
    loadOptions();
    sendString(replyto, "reloaded default options.");
  }
} // SST::loadOptionsFile

void SST::doCommand(ReadonlySequence<TextKey> &it, Fildes &replyto) {
  if (it.hasNext()) {
    Text ccode = it.next();
    dbg("Command code: %s", ccode.c_str());
    if (ccode == "set") {
      setParam(it, replyto);
    } else if (ccode == "get") {
      getParam(it, replyto);
    } else if (ccode == "quit") {
      quitProgram(it, replyto);
    } else if (ccode == "spew") {
      spew = atoi(it.next());
    }
  }
}

void SST::exitProgram(int Ereason) {
  shutdownProcess = Ereason;
  shutdown();//early response
} // SST::doCommand

void SST::ServePolledTimers(unsigned) {
  for (unsigned ti = timerService.quantity(); ti-- > 0;) {
    timerService[ti]->check();
  }
  if (!justTime) {//if justTime then ack() is called to get us here.
    timerSource.ack();//"may I have another, please?"
  }
}

unsigned SST::ticksForInterval(double seconds) {
  return timerSource.chunks(ratio(1.0, seconds));
}

bool SST::keepAlive() {
  if (spew > 3) { dbg("elapsed %g", looper.elapsed); }
  //do simple timed stuff
  if (timerSource.ack()) {
    ServePolledTimers(0);
//    if(spew>2) dbg("served time at %g", timertest.roll());
  }
  //then check for command input
  pollConsoles();

  if (shutdownProcess > 0) {
    //we can leave now, we've made a good faith effort to shut off the x-ray tube.
    looper.errornumber = shutdownProcess;
    return false;
  }

  return true;
} // SST::keepAlive

SST::SST(unsigned argc, char *argv[]) :
    Application(argc, argv),
    ConnectGroot(options),
    timerService(
        false), //arg false: the timers exist outside of the timerService, if true you get sigaborts on program exit.


    status("trace") {

  //in case we make this dynamically programmable:
  options.logicHz.onAnyChange([this]() {
    period = timerSource.chunks(options.logicHz);//this makes period be milliseconds, versus originally being ticks
  });

  status.preopened(STDOUT_FILENO, false);
}

int SST::prelims() {
  that = this;
  logArgs();
  logCwd();
  timerSource.setPeriod(
      0.001);//expecting milliseconds in so many places, will eventually make configurable once more structure is in place.

  if (failure(loadOptions())) {
    dbg("Running with intrinsic defaults, see debug log for proper filename.");
  } else {
    dbg("Options loaded OK");
  }

//  consoleCommand.receiver.rewind();

  if (options.console.enabled) {
    dbg("About to open console:");
    console.maxClients = options.console.allowed;
    console.init(options.console.interface.c_str(), options.console.port, nullptr);
    if (console.serve(options.console.backlog)) {
      dbg("Console listening on port %d (%X)", console.atPort(), console.atPort());
    } else {
      dbg("Console server did NOT start");//todo: print args
    }
  }
}

int SST::loadOptions() {
  Text hostname = this->hostname();
//  options.hostname = hostname;//record for generating initial file.
  DottedName optsname('.', hostname);
  optsname.parse("json");
  Text optsFileName = optsname.pack(FileNameConverter());
  return JsonFile(Stored::Groot(0)).loadFile(optsFileName);
}

void SST::pollConsole(Socketeer &sock) {
  if (sock.isBlocking()) {
    dbg("who re-enabled blocking?");
    return;
  }

  if (sock.read(receiver) < 0) {//append any new input, value <0 is an error code.
    if (!sock.isWaiting()) {
      //todo: depending upon specifics of error we might close the socket, to make room for a fresh connection.
    }
  }

  if (receiver.used() > 0) {//if something has been actually read
    unsigned termloc(BadIndex);
    if (commandPresent(&termloc)) {
      dbg("Console command: %s", receiver.internalBuffer());
//      doCommand(parsed,sock);
      sendString(sock, PRompt);
      //shift out data we just looked at.
      receiver.removeFirst(termloc);
    }
  }
} // SST::pollConsole

void SST::pollConsoles() {
  if (console.poll([this](Socketeer &sock) {
    pollConsole(sock);
  })) {
    Socketeer *newconsole(console.newest());
    if (newconsole) {//#B&S
      dbg("waking up new console with fd %d", newconsole->asInt());
      newconsole->write(reinterpret_cast<const u8 *>(signon), sizeof(signon));
    }
  }
} // SST::pollConsoles

void SST::consoleUpdate(Socketeer &sock, Indexer<u8> &p) {
  p.rewind();//siblings monkey with it.
  sock.write(p);
}

void SST::updateConsoles(Indexer<u8> &&ref) {
  console.forEach([this, &ref](Socketeer &sock) {
    consoleUpdate(sock, ref);
  });
}


void SST::shutdown() {
  //destructors will close ports
}

int SST::main() {
  if (int ercode = prelims()) {
    return ercode;
  }
  arglist.rewind();
  arglist.skip(1);//ditch program name (arg 0)

  CrossShift punt(arglist);
  while (punt.hasNext()) {
    doCommand(punt, status);
  }

  pid_t pid = getpid();
  dbg("pid: %ld\n", long(pid));//coercing type for platform portability

  writepid("pidfile");//in cwd while we are creating program, will be in /tmp when in production.


  dbg("timer fd: %d", timerSource.asInt());

  //setup event handlers.
  //timersources never fail to open on linux unless the OS runs out of file numbers
//  looper.watch(timerSource.asInt(), EPOLLIN, EventHandler(ServePolledTimers));
//  host.run(looper);

  justTime = true; //epoll is still broken, delivers useless host event, and ignores all other events. and on rpi corrupts the callbacks.
  //the above makes the run() below call keepAlive periodically

  //events are processed in one of the EventHandlers above, or not at all. Note that ServePolledTimers has a set of event handlers too.
  int exitcode = Application::run();//all activity from this point on is via callbacks arranged in the previous few lines.
  //we don't get here until the application has decided to quit.
  shutdown();//probably already called, but call it again to be sure.
  return exitcode;
}

bool SST::commandPresent(unsigned int *termloc) {
  auto scan = receiver.getHead();
  while (scan.hasNext()) {
    if (scan.next() == 10) {
      if (termloc) {
        *termloc = scan.ordinal() - 1;
      }
      return true;
    }
  }
  return false;
}
// SST::main



ConsoleOptions::ConsoleOptions(Storable &node) : Stored(node),
                                                 ConnectChild(enabled, true),
                                                 ConnectChild(port, 0x980f),
                                                 ConnectChild(backlog, 5),
                                                 ConnectChild(allowed, 2),
                                                 ConnectChild(interface,
                                                              "localhost") {//"localhost" is safest security wise, "0.0.0.0" is most promiscuous.
  //#nada
}


static SST *root = nullptr; //easy handle for debugger access to what is otherwise hidden.
/** the 'true' main is ConsoleApplication::main*/
int main(int argc, char *argv[]){
  SST app(unsigned(argc), argv);
  root=&app;
  if(app.failure(app.main())) {
    switch (app.errornumber) {//see SST::doHostCommand for matching these code to their actions.
    case EL3RST: //reboot
      system("sudo /bin/systemctl reboot");
      return -1;
    case EL3HLT: //halt system
      system("sudo /bin/systemctl halt");
      return -1;
    }
    wtf("Program exit code:%d, may mean: %s",app.errornumber,app.errorText());
  } else {
    wtf("Normal program exit");
  }
  return app.errornumber;
} // main
