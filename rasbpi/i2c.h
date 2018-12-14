#ifndef I2C_H
#define I2C_H

#include "fildes.h" //uses ioctl and write and read

/**
A minor annoyance from teh OS that we don'ttry to hide:
each device must have its own file descriptor opened.

*/

class I2C : public Fildes {
  unsigned busnumber;
  //msb aligned like most databooks.
  unsigned address;

public:
  I2C(unsigned busnumber, unsigned devaddress);
  bool connect();

};

#endif // I2C_H
