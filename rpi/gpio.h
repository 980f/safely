#ifndef GPIO_H
#define GPIO_H

#include "memorymapper.h"

/** raspbery pi version of making a pin look like a bool.
This module does NOT ensure that the gpio space is available, you must init the memory mapping module yourself.
*/
class GPIO {
  unsigned pinIndex;
  unsigned  mask; //32 bits per register
  unsigned offset;//

  static Mapped<unsigned>gpioBase;

  enum RegisterAsIndex {
    Function=0, //6 here
    SetBits=7,
    ClearBits=10,
    Read=13,
    //interrupt config stuff goes here
    PullerCode=37,
    PullerClock  //two regs follower PullerCode

  };

private:
  static void initModule();

public:
  GPIO();
  GPIO(unsigned pinIndex,unsigned af,unsigned pull);

  GPIO& connectTo(unsigned pinIndex);
  /* write bit */
  void operator =(bool value) const noexcept ;
  /* read bit */
  operator bool() const noexcept;
  /* flip bit */
  void toggle() const noexcept{
    *this=1-*this;
  }
  /** af=0 for input, 1 for output, remaining values are alternate function select */
  GPIO &configure(unsigned af);
  /** @param pull: -1 for down, +1 for up, 0 for let it float */
  GPIO& pullit(int pull);
};

#endif // GPIO_H
