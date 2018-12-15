

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
The I2C interface has a prefix byte compare to the bus protocol. The msbs are used, 0x80 means "not the last command" and 0x40 "this is data, else command" so 0x80 and 0x00 are commands, 0x40 leads data and there is never a need to 'continue' data.

The chip has 64 lines worth of ram regardless of what is attached. When the display is fewer lines one can use that extra ram for things like triple buffering the display.
*/

/** doing our own to ensure library smallness*/
struct PixelCoord {
  unsigned x[2];
  PixelCoord(unsigned x0=0, unsigned x1=0) {
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
  }
};

class SSD1306 {
public:
  /** attributes of the display */
  struct Display {
    unsigned segments = 128;
    unsigned commons = 32;
    bool swcapvcc = true;

    unsigned resetPin;
    unsigned i2c_bus = 1;    // 1 is rPi default, other is hat id interface which is to be avoided for some unknown reason.
    bool altaddress = false; // modules have a usually hardwired address bit. Theoretically two controllers could be used for a 128 X 128 display.
    /** the number of 'pages' of display ram. We are using terms from the manual even if they are stilted */
    unsigned pages() const { return (commons + 7) / 8; }
  };

  /** program memory with image of what will be put onto display.
   * The ram is dynamically allocated. This will be reworked with Buffer and Block from safely once arduino versions are tested.
   * Extra bytes are allocated at each end of the frame buffer for use by the I2C interface code.
*/
  struct FrameBuffer {
    const unsigned pixwidth;
    const unsigned pixheight;
    const unsigned stride;
    u8 *const fb;

    FrameBuffer(unsigned pixwidth, unsigned pixheight)
      : pixwidth(pixwidth), pixheight(pixheight), // bounds
        stride((pixwidth + 7) / 8),               // chunkiness
        fb(new u8[2 + stride * pixheight]) {
      //#nada
    }

    ~FrameBuffer() { delete[] fb; }
  };

  /** maintains offset and bit picker in tandem with logical pixel coordinates.
Stepping out of bounds results in writes being stifled, algorithms can be sloppy around the edges and not suffer from truncation. */
  class Pen {
  public:
    bool ink;
    PixelCoord logic;

  private:
    u8 mask;
    unsigned offset;
    FrameBuffer &fb;

  public:
    Pen(FrameBuffer &fb):fb(fb){
      jumpto({0,0});
    }

    void splot() {
      if (logic.x[1] < fb.pixheight && logic.x[0] < fb.pixwidth) {
        if (ink) {
          fb.fb[offset] |= mask;
        } else {
          fb.fb[offset] &= ~mask;
        }
      }
    }

    void jumpto(PixelCoord &&random){
      logic=random;
      offset=1+fb.stride*logic.x[1]+logic.x[0]/8;
      mask=1<<(logic.x[0]&7);
    }

    void step(bool which, int direction) {
      logic.step(which, direction);
      if (which) {
        offset += direction < 0 ? -fb.pixheight : fb.pixheight;
      } else {
        if (direction != 0) {
          if (direction < 0) {
            mask >>= 1;
            if (mask == 0) {
              mask = 0x80;
              --offset;
            }
          } else {
            mask <<= 1;
            if (mask == 0) {
              mask = 0x01;
              ++offset;
            }
          }
        }
      }
    }

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

  /** code is the value for the first byte. bits is the field width, bytes is the number of command bytes, 1,2,3 are allowed but not checked, arf is an additional bit shift, for when the lsb of a field is not the lsb of the operand byte. */
  struct Reg {
    unsigned pattern;

    const unsigned code;
    const unsigned bits;
    const unsigned bytes;
    const unsigned arf;

    const unsigned mask;    // = ((1 << bits) - 1);  // ones where operand bits are allowed
    const unsigned aligner; // = 8 * (bytes-1) + arf; // little endian machine, must nominally reverse byte order

    Reg(unsigned code, unsigned bits = 1, unsigned bytes = 1, unsigned arf = 0) : code(code), bits(bits), bytes(bytes), arf(arf), mask((1 << bits) - 1), aligner(8 * (bytes - 1) + arf) {
      operator=(0); // init pattern, handy for testing code.
    }

    /** masks value into legal range and shifts to where it belongs */
    Reg &operator=(unsigned value) {
      pattern = code | (((value & ~mask) << aligner));
      return *this;
    }

    /** caller ensures that this won't overflow by prechecking that there is room for the bytes of this+1*/
    u8 *operator()(u8 *buffer)const {
      *buffer++ = 0x80; // mark all as continuations, caller will clear bit on first byte
      *buffer++ = pattern;
      if (bytes > 1) {
        *buffer++ = pattern >> 8;
      }
      if (bytes > 2) {
        *buffer++ = pattern >> 16;
      }
      if (bytes > 3) {
        *buffer++ = pattern >> 24;
      }
      return buffer;
    }
  };

public:
  // address pointer for refresh.
  Reg windowSeg = {0x21, 6, 3, 8}; // actuall 2 6 bit operands each in own byte but the first is always 0 for our use so we can cheat.
  Reg windowCom = {0x22, 3, 3, 8}; // 0=horizontal like Epson printer, 1=Vertical the most natural, 2= not reasonable for serial interface.
  Reg allOn = {0xA4};              // 1= all pixels lit
  Reg inverseVideo = {0xA6};       // inverts data on way ito the video buffer, doesn't alter existing image
  Reg display = {0xAE};            // 1= show data
  Reg contrast = {0x81, 8, 2};
  Reg scrolling = {0x2E};       //+1 to enable else off
  Reg osc = {0xD5, 8, 2};       // actually two nibbles, high is osc freq, starts as 8, low is divide-1
  Reg precharge = {0xD9, 8, 2}; // actually two nibbles, high is phase 2 of timing low nibble phase 1.
  Reg vcomh = {0xDB, 3, 2, 4};  // some kind of reset trigger maybe?
  Reg muxratio = {0xA8, 6, 2};  // minimum of 15 not enforced, resets to all ones==63
  Reg compins = {0x2c0, 2, 2, 4};
  Reg something = {0x8C};        //!!WAG, hard coded 8D and no matching document.
                                 // viewport controls
  Reg memoryMode = {0x20, 2, 2}; // 0=horizontal like Epson printer, 1=Vertical the most natural, 2= not reasonable for serial interface.
  Reg hflip = {0xA0};
  Reg vflip = {0xC0, 4, 1, 3}; // C0 or C8
  // DA.5 swaps screen halves, DA.4 is "interleave"
  Reg displayOffset = {0xD3, 6, 2};
  Reg startLine = {0x40, 6, 1};

  /** you can init inline with braces: {128,64 and so on}*/
  SSD1306(Display &&displaydefinition) : oled(displaydefinition), dev(oled.i2c_bus, 0x3C + oled.altaddress), pages(oled.pages()) {}

  /** tell the OS we want to use this hardware */
  bool connect() {
    resetpin.beGpio(oled.resetPin, 0, 1); // deferred to ensure gpio access mechanism is fully init
    return dev.connect();                 // just gets permissions and such, doesn't hog the master.
  }

  /** write one byte of display data. */
  bool data(unsigned pixchunk) {
    u8 cmd[2];
    cmd[0] = 0x40;
    cmd[1] = pixchunk;
    return dev.write(cmd, 2);
  }

  bool send(const Reg &reg) {
    u8 cmd[reg.bytes + 1];
    reg(cmd);
    cmd[0] = 0; // there is just one.
    return dev.write(cmd, sizeof(cmd));
  }

  /** configure the display based on values stored via the constructor */
  bool begin() {
    reset();
    send(display = 0);

    osc = 0x80;
    muxratio = oled.commons - 1;
    displayOffset = 0;
    startLine = 0;
    something = 1;
    // page address mode according to values, ignore for now. command(vccmode == EXTERNAL ? 0x10 : 0x14);
    memoryMode = 1; // 980f preference, adafruit likes 0 here.
    hflip = 1;
    vflip = 1;
    compins = oled.commons >= 64 ? 1 : 0;                                 // 0..3
    precharge = oled.swcapvcc ? 0xF1 : 0x22;                              // is actually two nibbles and neither should be 0
    vcomh = 4;                                                            // manual offers 0,2,3 as valid setting, the 4 is from adafruit
    contrast = oled.commons >= 64 ? (oled.swcapvcc ? 0xCF : 0x9F) : 0x8F; //?anal excretion. The value can be set arbitrarily by user so maybe this is an extra param to begin?

    // now to pack all of that into one big happy I2C operation:
    u8 packer[64];                                                                                                                       // arbitrary at first, might cheat and use fb, else determine empirically what is needed, perhaps worst case.
                                                                                                                                         // send a wad:
    u8 *end = osc(muxratio(displayOffset(startLine(something(memoryMode(hflip(vflip(compins(precharge(vcomh(contrast(packer)))))))))))); // excuse my lisp ;)
    packer[0] = 0;
    dev.write(packer, packer - end);

    send(display = 1);
    return true;
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

  void refresh(const FrameBuffer &fb) {
    // set gdram pointer to 0, which appears to be a side effect of these two commands. Actually the implied 0's are the pointer set, but the command insists we also resend the limits.
    windowCom = fb.stride - 1;
    windowSeg = fb.pixheight - 1;

#ifdef __linux__
    // 1st attempt: send the whole nine yards, hence adding an extra byte to fb allocator.
    fb.fb[0] = 0x40; // we require the user allocate this byte to us. If they fail to then the display is shifted and they will figure that out eventually.
    dev.write(fb.fb, 1 + fb.stride * fb.pixheight);
#else
    const unsigned WireLimit = 32; // to stay compatibile with arduinos we must limit a transmission to 32 bytes including the 0x40 leader.
    unsigned bytesPer = 24;        //= (WireLimit-1) - ((WireLimit-1)%pages); //makes ripping not quite so bad, breathing instead of tearing.
    // could be 28 on X32 display.
    for (unsigned i = 0; i < length; i += bytesPer) { // 16 at a time
      Wire.beginTransaction();
      Wire.write(0x40);
      Wire.write(fb.fb + i, bytesPer);
      Wire.endTransaction();
    }
#endif
  }

  // one user can't stand for this to take longer than 15ms (ESP8266 will potentially lose wifi state or data)
  void eachMilli() {
    // reset prepare
    // reset active
    // reset take hold

    // send init stuff
  }
};
