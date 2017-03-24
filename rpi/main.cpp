#include "gpio.h"
#include "logger.h"
#include "unistd.h"

static Logger out("MAIN");
int main(int argc, char *argv[]){

  for(int argi=0;argi<argc;++argi){
    out("arg[%d]=%s",argi,argv[argi]);
  }
  if(MemoryMapper::init(true)){
    GPIO somebit(21);//todo: safe test bit  0..27 seem to be ok 21 is handy
    out("Created bit ");
    somebit.configure(1,0);//simple output
    out("Configure as output");
    while(1){
      somebit.toggle();
      out("Toggled it");
      sleep(1);
      somebit.toggle();
//      somebit=1;
      sleep(3);
//      somebit=0;
    }
  }
  return 0;
}
