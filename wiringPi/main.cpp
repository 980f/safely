#include "gpio.h"

int main(int argc, char *argv[])
{
  if(MemoryMapper::init(true)){
    GPIO somebit(21);//todo: safe test bit  0..27 seem to be ok 21 is handy
    somebit.configure(1,0);//simple output

    somebit.toggle();
  }
  return 0;
}
