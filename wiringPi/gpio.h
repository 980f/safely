#ifndef GPIO_H
#define GPIO_H


#include "memorymapper.h"

/** raspbery pi version of making a pin look like a bool */
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
  GPIO(unsigned pinIndex);
  /* write bit */
  void operator =(bool value) const noexcept ;
  /* read bit */
  operator bool() const noexcept;
  /* flip bit */
  void toggle() const noexcept{
    *this=1-*this;
  }
  /** af=0 for input, 1 for output, remaining values are alternate function select */
  void configure(unsigned af,unsigned pull);
};

#endif // GPIO_H
