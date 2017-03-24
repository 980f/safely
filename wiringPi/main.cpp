#include "gpio.h"
#include "logger.h"

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
    somebit.toggle();
    out("Toggled it");
  }
  return 0;
}
