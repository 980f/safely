#include "i2c.h"

#include "logger.h"
#include <linux/i2c-dev.h> //ioctl wrapper
#include <linux/i2c.h>
#include "fcntlflags.h" //O_ stuff for file open

SafeLogger(i2c,0);


I2C::I2C(unsigned busnumber, unsigned devaddress):Fildes("I2C"),
  busnumber(busnumber),address(devaddress){
  //defer connect to a separate call, so that we can statically construct if we wish.
}

bool I2C::connect(){
  char devname[11]="/dev/i2c-x";//we will limit ourselves to the first 10 busses
  devname[9]='0'+busnumber;
  devname[10]=0;//nulterm

  if(open(devname,O_RDWR)){
    if(ioctl(I2C_SLAVE, address>>1)){//we accept 8 bit addresses as they are what is most ccommonly published and what appear on bus watchers.
      //worked OK, set some status for fail quick on other calls.
      return true;
    } else {
      i2c("connect failed due to %s",errorText()) ;
    }
  } else {
    i2c("open failed due to %s",errorText()) ;
  }
  return false;
}
