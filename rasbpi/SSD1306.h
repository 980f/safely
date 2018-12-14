

const unsigned I2C_ADDRESS = 0x3C; //# 011110+SA0+RW - 0x3C or 0x3D
//  enum Cmd {
//       SETCOMPINS = 0xDA,
//      SETLOWCOLUMN = 0x00,
//      SETHIGHCOLUMN = 0x10,

//      COLUMNADDR = 0x21,
//      PAGEADDR = 0x22,

/////# Scrolling constants
//      SET_VERTICAL_SCROLL_AREA = 0xA3,
//      RIGHT_HORIZONTAL_SCROLL = 0x26,
//      LEFT_HORIZONTAL_SCROLL = 0x27,
//      VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29,
//      VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A,

//};

#include "dout.h" //reset pin
#include "i2c.h"

/**

if set to vertical addressing mode then a natural mapping can be made of increasing bit address across the 32/64 dimension from top to bottom or right to left. This could be called 3rd quandrant addressing which while uncommon in CRT controllers works just as well as 1st or 4th quadrant attitudes.
Especially if you Semitic ;)
To get 4th quadrant such as VGA and all IBM PC adaptors one complements the coordinate, and that is something that the chip itself can help with.
So on to the frame buffer.
The I2C interface has a prefix byte compare to the bus protocol. The msbs are used, 0x80 means "there will be multiple commands" and 0x40 "this is data, else command" so 0x80 and 0x00 are commands, 0x40 leads data and there is never a need to 'continue' data.


*/
class SSD1306 {

public:
  const unsigned segments;
  const unsigned commons;
  const bool swcapvcc;
private:
  unsigned pinIndex;//,unsigned af,unsigned pull);//make a gpio
  Dout resetpin;
  I2C dev;
  unsigned pages;
  u8 *buffer;


  /** code is the value for the first byte. bits is the field width, bytes is the number of command bytes, 1,2,3 are allowed but not checked, arf is an additional bit shift, for when the lsb of a field is not the lsb of the operand byte. */
  template <unsigned code, unsigned bits = 1, unsigned bytes = 1, unsigned arf = 0> struct Register {
    u8 cmd[1+bytes];//0th byte is 0 for single command, 0x80 if it is followed by another, will abuse other bit as 'dirty'
    Register(){
      operator =(0);
    }
    enum {
      mask = ((1 << bits) - 1),  // ones where operand bits are allowed
      aligner = 8 * (bytes-1) + arf, // little endian machine, must nominally reverse byte order
    };

    void operator=(unsigned value) {
      cmd[0]=1;//dirtybit
      unsigned junk=code | (((value & ~mask) << aligner));
      cmd[1]=junk;
      cmd[2]=junk>>8;
    }
  };

public:
  //address pointer for refresh.
  Register<0x21, 6, 3, 8> windowSeg; // actuall 2 6 bit operands each in own byte but the first is always 0 for our use so we can cheat.
  Register<0x22, 3, 3, 8> windowCom; // 0=horizontal like Epson printer, 1=Vertical the most natural, 2= not reasonable for serial interface.
  Register<0xA4> allOn;        // 1= all pixels lit
  Register<0xA6> inverseVideo; // inverts data on way ito the video buffer, doesn't alter existing image
  Register<0xAE> display;      // 1= show data
  Register<0x81, 8, 2> contrast;

  Register<0x2E> scrolling; //+1 to enable else off

  Register<0xD5, 8, 2> osc;
  Register<0xD9, 8, 2> precharge;
  Register<0xDB, 3, 2, 4> vcomh;
  Register<0xA8, 6, 2> muxratio; // minimum of 15 not enforced, resets to all ones==63
  Register<0x2c0, 2, 2, 4> compins;
  Register<0x8C> chargePump; //!!WAG, hard coded 8D and no matching document.
//viewport controls
  Register<0x20, 2, 2> memoryMode;   // 0=horizontal like Epson printer, 1=Vertical the most natural, 2= not reasonable for serial interface.
  Register<0xA0> hflip;
  Register<0xC0, 4, 1, 3> vflip;
  Register<0xD3, 6, 2> displayOffset;
  Register<0x40, 6, 1> startLine;


  /** typically 128 segments and 32 or 64 commons.
rst is the iopin designator for the hardware reset, i2c-bus picks an interface, i2c_address is one of two supported by the chip, should reduce to a boolean. */
  SSD1306(unsigned segments, unsigned commons, unsigned rst, bool swcapvcc,unsigned i2c_bus = 1, unsigned i2c_address = I2C_ADDRESS)
    : segments(segments), commons(commons),
      pinIndex(rst), // will need to remap value somewhere
      swcapvcc(swcapvcc),
      dev(i2c_bus, i2c_address),
      pages(commons / 8),  //maydo: guard against a non multiple of 8 commons, also idiot check other numbers.
      buffer(new u8[segments * pages]) {}

  ~SSD1306() { delete[] buffer; }

  /** tell the OS we want to use this hardware */
  bool connect() {
    resetpin.beGpio(pinIndex,0,1);//deferred to ensure gpio access mechanism is fully init
    return dev.connect(); //just gets permissions and such, doesn't hog the master.
  }

  /** write one byte of display data. */
  bool data(unsigned pixchunk) {
    unsigned pattern = 0x40 | (pixchunk << 8);
    dev.write(reinterpret_cast<u8*>(&pattern), 2); //+1 for the C0/D byte
  }

  bool begin() {
    //        """Initialize display."""
    //        # Reset and initialize display.
    reset();
    display = 0;
    osc = 0x80;
    muxratio = commons - 1;
    displayOffset = 0;
    startLine = 0;
    // looks funky:
    chargePump = 1;
    // page address mode according to values, ignore for now. command(vccmode == EXTERNAL ? 0x10 : 0x14);
    memoryMode = 1; // 980f preference, adafruit likes 0 here.
    hflip = 1;
    vflip = 1;
    compins = commons >= 64 ? 1 : 0;                            // 0..3
    contrast = commons >= 64 ? (swcapvcc ? 0xCF : 0x9F) : 0x8F; //?anal excretion

    precharge = swcapvcc ? 0xF1 : 0x22; // is actually two nibbles and neither should be 0
    vcomh = 4;                          // manual offers 0,2,3 as
    display = 1;
  }


  void reset() {
    //        """Reset the display."""
    //        # Set reset high for a millisecond.
    resetpin = 1;
    // do this via callbacks!
    //+        MicroSecondtime.sleep(0.001)
    //        # Set reset low for 10 milliseconds.
    resetpin = 0;
    //+        time.sleep(0.010)
    //        # Set reset high again.
    resetpin = 1;
  }

  //        """Write display buffer to physical display."""

  void refresh() {
    // set gdram pointer to 0, which appears to be a side effect of these two commands. Actually the implied 0's are the pointer set, but the command insists we also resend the limits.
    windowCom = pages - 1;
    windowSeg = segments - 1;
    //        # Write buffer data.
    for (unsigned i = 0; i < sizeof(buffer); i += 16) { // 16 at a time
      dev.write(buffer + i, 16);                        // todo: prefix with 0x40
    }
  }

  //one user can't stand for this to take longer than 15ms (ESP8266 will potentially lose wifi state or data)
  void eachMilli(){
    //reset prepare
    //reset active
    //reset take hold

    //send init stuff

  }

};
