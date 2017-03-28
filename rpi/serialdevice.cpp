#include "serialdevice.h"

#include "fcntlflags.h"
#include <termios.h>
#include <unistd.h>

#include "logger.h"

SerialDevice::SerialDevice(){
  //#nada
}

bool SerialDevice::connect(const SerialConfiguration &cfg){
  fd.open(cfg.device.c_str(),O_NOCTTY|O_NONBLOCK|O_RDWR);
  if(!fd.isOpen()){
    dbg("Failed to open port %s, error:%s",cfg.device.c_str(),fd.errorText());
  }

  struct termios tty;
  EraseThing(tty);//tradition, tradition .... tra.di.tion.

  if (fd.failed(tcgetattr(fd.asInt(), &tty))) {
    dbg("Error from tcgetattr: %s", fd.errorText());
    return false;
  }

  cfsetospeed(&tty, cfg.baud);
  cfsetispeed(&tty, cfg.baud);
  cfmakeraw(&tty);//N81, make changes below
  tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
  tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
  tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

  /* 0,0 here makes this port non-blocking */
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 0;

  if (fd.failed(tcsetattr(fd, TCSANOW, &tty))) {
    dbg("Error from tcsetattr: %s", fd.errorText());
    return false;
  }
  return true;
}


void SerialDevice::close(){
  fd.close();
}

SerialConfiguration::SerialConfiguration(Storable &node):Stored(node),
  ConnectChild(device,"/dev/ttyUSB0"),
  ConnectChild(baud,115200),
  ConnectChild(parity,0),//"none",
  ConnectChild(bits,8),
  ConnectChild(stop,1){
  //#nada
}
