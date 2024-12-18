#pragma once

#include <cstring>

#include "dout.h" //reset pin
#include "i2c.h"  //safely wrapper, not the OS's

/**

if set to vertical addressing mode then a natural mapping can be made of increasing bit address across the 32/64 dimension from top to bottom or right to left. This could be called 3rd quandrant addressing which while uncommon in CRT controllers works just as well as 1st or 4th quadrant attitudes.
Especially if you are Semitic and laying on your side ;)
To get 4th quadrant such as VGA and all IBM PC adaptors one complements the coordinate, and that is something that the chip itself can help with.
So on to the frame buffer.
The I2C interface has a prefix byte compare to the '8080' bus protocol.
The msbs are used, 0x80 means "not the last command" and 0x40 "this is data, else command" so 0x80 and 0x00 are commands, 0x40 leads data and there is never a need to 'continue' data.

The chip has 64 lines worth of ram regardless of what is attached. When the display is fewer lines one can use that extra ram for things smooth scrolling, one can alter the start-of-screen address.
*/

/** doing our own drawing classes to ensure library smallness, and to expedite axis swapping */
struct PixelCoord {
  unsigned x[2];

  PixelCoord(unsigned x0 = 0, unsigned x1 = 0) {
    x[0] = x0;
    x[1] = x1;
  }

  // default copy and assignment work just fine

  /** move one step along the given access in the given direction, ignoring the magnitude of the direction. For direction==0 don't move at all.*/
  PixelCoord &step(bool which, int direction) {
    if (direction == 0) {
      return *this;
    }
    x[which] += direction < 0 ? -1 : 1;
    return *this;
  }
};

/** combined class of display controls and namespace for rendering algorithms customized for the structure of the frame buffer. */
class SSD1306 {
public:
  /** attributes of the display */
  struct Display {
    // this is the coordinate that is 1:1 with pixels, usually horizontal or X
    unsigned segments = 128;
    // this is the coordinate that is managed 8 at a time (whether you like that or not), usually vertical or Y
    unsigned commons = 32;
    // this is some power supply configuration, the false is 'external'. The name suggests the chip has a charge pump to generate voltage needed by the OLED.
    bool swcapvcc = true;

    // for easiest control it is good to have a hard reset into the device. This number depends upon platform, you may have to do some research to figure out what your actual pin is.
    unsigned resetPin;
    // on the RPi there is more than one I2C port, ditto for some arduinos.
    unsigned i2c_bus = 1; // 1 is rPi default, other is hat id interface which is to be avoided for some unknown reason.
    // there are two I2C addresses the chip supports. This matches the "SA0" pin.
    bool altaddress = false; // modules have a usually hardwired address bit. Theoretically two controllers could be used for a 128 X 128 display.

    /** the number of 'pages' of display ram used. We are using terms from the manual even if they are stilted */
    unsigned pages() const {
      return (commons + 7) / 8;
    }
  };

  enum Code {
    DataMarker = 0x40, //first byte of I2C payload, indicates that the remainder goes to ram.
  };

  /** processor memory with image of what will be put onto display.
   *
   * coordinates on the chip are COMmmons, the narrower range, and SEGments the wider range.
   */
  struct FrameBuffer {
    const unsigned comspan;
    const unsigned segspan;
    // bytes per increment of the seg coordinate.
    const unsigned stride;
    const unsigned databytes;
    // The ram is dynamically allocated. This will be reworked with Buffer and Block from safely once arduino versions are tested.
    u8 *fb;

    FrameBuffer(unsigned pixwidth, unsigned pixheight = 128);

    ~FrameBuffer() {
      delete[] fb;
    }

    void clear(bool ink = false) {
      memset(fb + 1, ink ? 255 : 0, databytes); //leave control bytes unchanged.
    }

    u8 &operator()(unsigned page, unsigned segment) {
      return fb[1 + page + stride * segment];
    }

  public:
    void markAsData() const {
      fb[0] = DataMarker;
    };
  };

  /** maintains offset and bit picker in tandem with logical pixel coordinates. This is for slower processors, especially ones with no native divide instruction.
   * Even on systems with a divide instruction it might be slower than this.
   * Stepping out of bounds results in writes being stifled, algorithms can be sloppy around the edges and not suffer from truncation. */
  class Pen {
  public:
    bool ink;
    PixelCoord logic;

  private:
    u8 mask;
    unsigned offset;
    FrameBuffer &fb;

  public:
    Pen(FrameBuffer &fb) : fb(fb) {
      jumpto({0, 0});
    }

    /** set a pixel */
    void splot();

    /** move coordinates, but don't alter image */
    void jumpto(PixelCoord &&random);

    /** just move one cell in any of 4 directions, with a 5th option to not move at all.
     * we'll add step0 and step1 functions later that always step but just along the buitin-to-name axis.
     */
    void step(bool which, int direction);

    /** todo: discuss whether we should mark the pixel before we step vs. after */
    void draw(bool which, int direction) {
      step(which, direction);
      splot();
    }
  };

public:
  const Display oled;

private:
  Dout resetpin;
  I2C dev;
  unsigned pages;

  /** A base class so that we can send singles simply via a reference. It can be used to send up to 4 bytes of content to the chip. */
  struct Register {
    unsigned pattern;
    const unsigned bytes;

    /** masks value into legal range and shifts to where it belongs */
    virtual Register &operator=(unsigned value) = 0;

    Register(unsigned bytes);

    virtual ~Register() = default; //to stifle warnings
    /** caller ensures that this won't overflow by prechecking that there is room for the bytes of this+1*/
    u8 *operator()(u8 *buffer) const;
  };

  /** code is the value for the first byte. bits is the field width, bytes is the number of command bytes, 1,2,3 are allowed but not checked, arf is an additional bit shift, for when the lsb of a field is not the lsb of the operand byte. */
  template<unsigned code, unsigned bits = 1, unsigned numbytes = 1, unsigned arf = 0> struct Reg : public Register {
    enum {
      mask = ((1 << bits) - 1), // ones where operand bits are allowed
      aligner = 8 * (numbytes - 1) + arf, // little endian machine, must nominally reverse byte order
    };

    Reg() : Register(numbytes) {}

    /** masks value into legal range and shifts to where it belongs */
    Register &operator=(unsigned value) {
      pattern = code | ((value & mask) << aligner);
      return *this;
    }
  };

private:
  enum BackgroundActions {
    Idle,
    StartReset, // set to a 1, this ensures it spends some time there.
    ActiveReset, // set to 0, this ensures it is long enough.
    SendInit, // set to 1,
  } bgact = Idle;

  unsigned bgdelay = 0;

private: //todo: inline each of these in accessor functions which convert from application data types to that of the display

  Reg<0x21, 6, 3, 8> windowSeg; // address pointer for refresh.
  //  Reg windowSeg = {0x21, 6, 3, 8}; // actually 2 6 bit operands each in own byte but the first is always 0 for our use so we can cheat.
  Reg<0x22, 3, 3, 8> windowCom; // see above, smaller arguments on page axis.
  Reg<0xA4> allOn; // 1= all pixels lit
  Reg<0xA6> inverseVideo; // inverts data on way ito the video buffer, doesn't alter existing image
  Reg<0xAE> display; // 1= show data
  Reg<0x81, 8, 2> contrast; // the adafruit code has strange ideas of suggested values for init.
  Reg<0x2E> scrolling; // to enable else off
  Reg<0xD5, 8, 2> osc; // actually two nibbles, high is osc freq, starts as 8, low is divide-1
  Reg<0xD9, 8, 2> precharge; // actually two nibbles, high is phase 2 of timing low nibble phase 1.
  Reg<0xDB, 3, 2, 4> vcomh; // some kind of reset trigger maybe?
  Reg<0xA8, 6, 2> muxratio; // minimum of 15 not enforced, resets to all ones==63
  Reg<0x2c0, 2, 2, 4> compins;
  Reg<0x8C> something; //!!WAG, hard coded 8D and no matching document.
  // viewport controls
  Reg<0x20, 2, 2> memoryMode; // 0=horizontal like Epson printer, 1=Vertical the most natural, 2= not reasonable for serial interface.
  Reg<0xA0> hflip;
  Reg<0xC0, 4, 1, 3> vflip; // C0 or C8
  Reg<0xD3, 6, 2> displayOffset;
  Reg<0x40, 6, 1> startLine;

public:
  /** you can init inline with braces: {128,64 and so on}*/
  SSD1306(const Display &&displaydefinition);

  /** tell the OS we want to use this hardware */
  bool connect();

  /** send a solitary command.*/
  bool send(const Register &reg, bool asdata = false);

  /** configure the display based on values stored via the constructor */
  void begin();

  /** @returns whether it can be talked to profitably. False when going through initialization sequence or not configured*/
  int busy() {
    if (!dev.isOpen()) {
      return -1;
    }
    if (bgact != Idle) {
      return 1;
    }
    return 0;
  }

  void setContrast(unsigned bytish);

  void refresh(const FrameBuffer &fb);

  /**for access to trouble codes */
  const Fildes &lowlevel;

private:
  // called back via procedure started with reset()
  void sendInit();

  void reset();

  // for when our application is polled by something like an NRF radio chip.
  // todo: rework with NanoSecond return mechanism once we have that for Arduino.
  void eachMilli();
};
