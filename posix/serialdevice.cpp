//"(C) Andrew L. Heilveil, 2017"
#include "serialdevice.h"

#include "fcntlflags.h"
#include <termios.h>
#include <unistd.h>

#include "logger.h"

SerialDevice::SerialDevice() : fd("SerialPort"){
  //#nada
}

speed_t mapBaud(double baud){
  //stupid ancient interface (grumble, whine)
  static unsigned lowset [] = {    0,    50,    75,    110,    134,    150,    200,    300,    600,    1200,    1800,    2400,    4800,    9600,    19200,    38400  };
  static unsigned highset[] = {    57600,    115200,    230400,    460800,    500000,    576000,    921600,    1000000,    1152000,    1500000,    2000000,    2500000,    3000000,   3500000,    4000000  };
  for(unsigned bi = countof(lowset); bi-->0; ) {
    if(baud==lowset[bi]) {
      return bi;
    }
  }
  for(unsigned bi = countof(highset); bi-->0; ) {
    if(baud==highset[bi]) {
      return bi + B57600;//0010001;
    }
  }
  return 0;
} // mapBaud

bool SerialDevice::connect(const SerialConfiguration &cfg){
  fd.open(cfg.device.c_str(),O_NOCTTY | O_NONBLOCK | O_RDWR);
  if(!fd.isOpen()) {
    dbg("Failed to open port %s, error:%s",cfg.device.c_str(),fd.errorText());
    return false;
  }

  struct termios tty;
  EraseThing(tty);//tradition, tradition .... tra.di.tion.

  if (fd.failed(tcgetattr(fd.asInt(), &tty))) {
    dbg("Error from tcgetattr: %s", fd.errorText());
    return false;
  }

  speed_t baud = mapBaud(cfg.baud);//only works for well known bauds.

  if(fd.failed(cfsetospeed(&tty, baud))) {
    dbg("Error from cfsetospeed: %s", fd.errorText());
  }
  if(fd.failed(cfsetispeed(&tty, baud))) {
    dbg("Error from cfsetispeed: %s", fd.errorText());
  }
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
} // SerialDevice::connect

void SerialDevice::close(){
  fd.close();
}

SerialDevice::Pin *SerialDevice::makePin(SerialDevice::Pin::Which one, bool invert){
  return new SerialDevice::Pin(fd,one,invert);
}

SerialConfiguration::SerialConfiguration(Storable &node) : Stored(node),
  ConnectChild(device,"/dev/ttyUSB0"),
  ConnectChild(baud,115200),
  ConnectChild(parity,0),//"none",
  ConnectChild(bits,8),
  ConnectChild(stop,1){
  //#nada
}

//  digital output stuff
  #include <sys/ioctl.h>

/*
 *  TIOCMSET  const int *argp  Set the status of modem bits.
 *  TIOCMBIC  const int *argp  Clear the indicated modem bits.
 *  TIOCMBIS  const int *argp  Set the indicated modem bits.
 *
 *  The following bits are used by the above ioctls:
 *
 *      TIOCM_LE        DSR (data set ready/line enable)
 *      TIOCM_DTR       DTR (data terminal ready)
 *      TIOCM_RTS       RTS (request to send)
 *      TIOCM_ST        Secondary TXD (transmit)
 *      TIOCM_SR        Secondary RXD (receive)
 *      TIOCM_CTS       CTS (clear to send)
 *      TIOCM_CAR       DCD (data carrier detect)
 *      TIOCM_CD         see TIOCM_CAR
 *      TIOCM_RNG       RNG (ring)
 *      TIOCM_RI         see TIOCM_RNG
 *      TIOCM_DSR       DSR (data set ready)
 *
 */
//bool SerialDevice::updateDigitalOutputs(const SerialDevice::DigitalOutputs &bits){
//  int pattern=0;
//  if(bits.DSR){
//    pattern |= TIOCM_LE;
//  }
//  if(bits.CTS){
//    pattern |= TIOCM_CTS;
//  }
//  //the following are only input on most systems, but ioclt doesn't care
//  if(bits.DCD){
//    pattern |= TIOCM_CD;
//  }
//  if(bits.RI){
//    pattern |= TIOCM_RI;
//  }
//  return fd.ok(ioctl(fd,TIOCMSET, &pattern));
//}


SerialDevice::Pin::operator bool() noexcept {
  int pattern;
  if(fd.ok(ioctl(fd,TIOCMGET, &pattern))) {
    pattern &= this->pattern;//pick our bit
    return invert ? pattern ==0 : pattern!=0;
  } else {
    return lastSet;//guess
  }
}

SerialDevice::Pin::Pin(const Fildes &fd, SerialDevice::Pin::Which one, bool invert) :
  fd(fd),
  which(one), //kept for debug
  invert(invert),
  lastSet(!invert){//any value is better than random.
  switch (one) {
  case Dtr:
    pattern = TIOCM_DTR;
    break;
  case Rts:
    pattern = TIOCM_RTS;
    break;
  //and these are usually inputs, but ioctl doesn't care
  case Dsr:
    pattern = TIOCM_LE;
    break;
  case Cts:
    pattern = TIOCM_CTS;
    break;
  case Dcd:
    pattern = TIOCM_CD;
    break;
  case Ri:
    pattern = TIOCM_RI;
  default:
    pattern = 0;
    break;
  } // switch
}

bool SerialDevice::Pin::operator =(bool on) noexcept {
  lastSet = on;
  bool level = invert ? !on : on;
  if(fd.ok(ioctl(fd,level ? TIOCMBIS : TIOCMBIC, &pattern))) {
    return true;
  } else {
    return false;
  }
}

SPIO::SPIO(SerialDevice::Pin &raw) : raw(raw){
}

void SPIO::operator =(bool value) noexcept {
  raw = value;
}

SPIO::operator bool() noexcept {
  return raw;
}

void SPIO::toggle() noexcept {
  raw.toggle();
}
