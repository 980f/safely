#include "gpio.h"
#include "logger.h"
//#include "unistd.h"
#include "epoller.h"
#include "errno.h" //firstly for polling errors

#include "application.h"

static Logger out("MAIN");

GPIO watchDogReset(21);//todo: safe test bit  0..27 seem to be ok 21 is handy

int main(int argc, char *argv[]){
  Application app(argc,argv);
  app.logArgs();
  app.logCwd();
  if(MemoryMapper::init(true)){
    watchDogReset.configure(1,0);//simple output
    watchDogReset=1;

    int exitcode=app.run();
    return exitcode;

  } else {
    if(MemoryMapper::Mmap){
      out("failed to acquire GPIO access, MemoryMapper::init failed. \nThis depends upon a link named \"safe-memory\", pointing to either /dev/gpiomem or a file big enough to simulate the RPI iospace");
      out("dd bs=160 count=1 if=/dev/zero of=dummy-memory");
      out("");
      out("If you get a SIGBUS then you may not have rights to run this program");


      return MemoryMapper::Mmap->getError();
    } else {
      out("couldn't allocate a memory mapper");
      return ENOMEM;
    }
  }
  return 0;
}
