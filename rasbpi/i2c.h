#ifndef I2C_H
#define I2C_H

#include "fildes.h" //uses ioctl and write and read

/**
A minor annoyance from the OS that we don't try to hide:
each device must have its own file descriptor opened as the I2C address is part of the device interface rather than of the packet to send.

*/

class I2C : public Fildes {
  /** which I2C bus. */
  unsigned busnumber;
  /** always even, msb aligned like most databooks. (8-bit addressing, vs arduino 7-bit addressing) */
  unsigned address;

public:
    /** just record relevant numbers, call connect() to gain access to I2C */
  I2C(unsigned devaddress,unsigned busnumber=0);
  /** get a file descriptor, @returns whether that was successful. You can also check isOpen() after the fact. */
  bool connect();
  /** and reading and writing are done as to a file, but leave out the device address from your block to send. */
};

#endif // I2C_H
