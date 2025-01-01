#include "SSD1306.h"

SSD1306::FrameBuffer::FrameBuffer(unsigned pixwidth, unsigned pixheight) : comspan(pixwidth), segspan(pixheight), // bounds
  stride((pixwidth + 7) / 8), // chunkiness
  databytes(stride * pixheight),
  fb(new uint8_t[2 + databytes]) { //* Extra bytes are allocated at each end of the frame buffer for use by the I2C interface code.
  //#nada
}

SSD1306::SSD1306(const Display &&displaydefinition) : oled(displaydefinition), dev(oled.i2c_bus, 0x3C + oled.altaddress), pages(oled.pages()), lowlevel{dev} {}

bool SSD1306::connect() {
  if (oled.resetPin != ~0U) {
    resetpin.beGpio(oled.resetPin, 0, 1); // deferred to ensure gpio access mechanism is fully init
  }
  return dev.connect(); // just gets permissions and such, doesn't hog the master.
}

bool SSD1306::send(const Register &reg, bool asdata) {
  uint8_t cmd[reg.bytes + 1];
  reg(cmd); //reg's exist to poke their values into a buffer.
  cmd[0] = asdata ? DataMarker : 0;
  return dev.write(cmd, sizeof(cmd));
}

void SSD1306::begin() {
  if (oled.resetPin != ~0U) { //initial test hardware doesn't have a reset pin. I wasted my time coding all that timing stuff!
    reset();
  } else {
    sendInit();
    bgact = Idle;
  }
  // adafruit disabled the display, but so does reset. Perhaps that was in case there was no reset pin in some mechanical arrangements.
  //the rest of the adafruit equivalent code is in sendInit();
}

void SSD1306::setContrast(unsigned bytish) {
  send(contrast = bytish);
}

void SSD1306::refresh(const FrameBuffer &fb) {
  // set gdram pointer to 0, which appears to be a side effect of these two commands. Actually the implied 0's are the pointer set, but the command insists we also resend the limits.
  windowCom = fb.stride - 1;
  windowSeg = fb.segspan - 1;

#ifdef __linux__
  // 1st attempt: send the whole nine yards, hence adding an extra byte to fb allocator.
  fb.markAsData(); // we require the user allocate this byte to us. If they fail to then the display is shifted and they will figure that out eventually.
  dev.write(fb.fb, 1 + fb.stride * fb.segspan);
#else
  const unsigned WireLimit = 32; // to stay compatibile with arduinos we must limit a transmission to 32 bytes including the 0x40 leader.
  unsigned bytesPer = 24;        //= (WireLimit-1) - ((WireLimit-1)%pages); //makes ripping not quite so bad, breathing instead of tearing.
  // could be 28 on X32 display.
  for (unsigned i = 0; i < length; i += bytesPer) { // 16 at a time
    Wire.beginTransaction();
    Wire.write(DataMarker);
    Wire.write(fb.fb + i, bytesPer);
    Wire.endTransaction();
  }
#endif
}

void SSD1306::sendInit() {
#if MassSend ==1
  osc = 0x80;
  muxratio = oled.commons - 1;
  displayOffset = 0;
  startLine = 0;
  something = 1;
  // page address mode according to values, ignore for now. command(vccmode == EXTERNAL ? 0x10 : 0x14);
  memoryMode = 1; // 980f preference, adafruit likes 0 here. 1 = 90' rotated 3rd quandrant, fix in Pen class.
  hflip = 1;      // make these two configurable, or after-the-fact adjustable.
  vflip = 1;
  compins = oled.commons >= 64 ? 1 : 0;                                 // 0..3
  precharge = oled.swcapvcc ? 0xF1 : 0x22;                              // is actually two nibbles and neither should be 0
  vcomh = 4;                                                            // manual offers 0,2,3 as valid setting, the 4 is from adafruit
  contrast = oled.commons >= 64 ? (oled.swcapvcc ? 0xCF : 0x9F) : 0x8F; //?anal excretion. The value can be set arbitrarily by user so maybe this is an extra param to begin?
  display = 1;


  // now to pack all of that into one big happy I2C operation:
  uint8_t packer[64];                                                                                                                        // arbitrary at first, might cheat and use fb, else determine empirically what is needed, perhaps worst case.
  /* send a wad: each passes along a pointer after writing a few bytes with it.
 they will get sent right to left, if order is relevant then do follower(precedent(..)*/
  uint8_t *last = osc(muxratio(displayOffset(startLine(something(memoryMode(hflip(vflip(compins(precharge(vcomh(contrast(packer)))))))))))); // excuse my lisp ;)
  uint8_t *end = display(last); //34 command bytes on 1st run.
  *last = 0; // the above functions prefix the continuation flag, we eliminate that with this line.
  dev.write(packer, end-packer);
  // todo: log actual length to see if we can reduce packer's allocation, or abuse the frame buffer and clear screen.
#else
  send(osc = 0x80);
  send(muxratio = oled.commons - 1);
  send(displayOffset = 0);
  send(startLine = 0);
  send(something = 1);
  // page address mode according to values, ignore for now. command(vccmode == EXTERNAL ? 0x10 : 0x14);
  send(memoryMode = 1); // 980f preference, adafruit likes 0 here. 1 = 90' rotated 3rd quandrant, fix in Pen class.
  send(hflip = 1); // make these two configurable, or after-the-fact adjustable.
  send(vflip = 1);
  send(compins = oled.commons >= 64 ? 1 : 0); // 0..3
  send(precharge = oled.swcapvcc ? 0xF1 : 0x22); // is actually two nibbles and neither should be 0
  send(vcomh = 4); // manual offers 0,2,3 as valid setting, the 4 is from adafruit
  send(contrast = oled.commons >= 64 ? (oled.swcapvcc ? 0xCF : 0x9F) : 0x8F); //?anal excretion. The value can be set arbitrarily by user so maybe this is an extra param to begin?
  send(display = 1);
#endif
}

void SSD1306::reset() {
  resetpin = 1;
  bgdelay = 2; // in case first is short
  bgact = StartReset;
}

void SSD1306::eachMilli() {
  switch (bgact) {
    case Idle:
      return;
    case StartReset:
      if (--bgdelay == 0) {
        resetpin = 0;
        bgdelay = 10;
        bgact = ActiveReset;
      }
      return;
    case ActiveReset:
      if (--bgdelay == 0) {
        resetpin = 1;
        bgdelay = 1;
        bgact = SendInit;
      }
      return;
    case SendInit:
      if (--bgdelay == 0) {
        sendInit();
        bgact = Idle;
      }
      return;
  }
}

SSD1306::Register::Register(unsigned bytes) : pattern(0), bytes(bytes) {}

uint8_t *SSD1306::Register::operator()(uint8_t *buffer) const {
  *buffer++ = 0x80; // mark all as continuations, caller will clear bit on first byte
  *buffer++ = uint8_t(pattern);
  if (bytes > 1) {
    *buffer++ = uint8_t(pattern >> 8);
  }
  if (bytes > 2) {
    *buffer++ = uint8_t(pattern >> 16);
  }
  if (bytes > 3) {
    *buffer++ = uint8_t(pattern >> 24);
  }
  return buffer;
}

void SSD1306::Pen::splot() {
  if (logic.x[1] < fb.segspan && logic.x[0] < fb.comspan) {
    if (ink) {
      fb.fb[offset] |= mask;
    } else {
      fb.fb[offset] &= ~mask;
    }
  }
}

void SSD1306::Pen::jumpto(PixelCoord &&random) {
  logic = random;
  offset = 1 + fb.stride * logic.x[1] + logic.x[0] / 8; // initial 1+ is for the I2C to insert a command at front of buffer before sending it. Presumes we send all 1k in one command.
  mask = 1 << (logic.x[0] & 7);
}

void SSD1306::Pen::step(bool which, int direction) {
  logic.step(which, direction);
  if (which) {
    offset += direction < 0 ? -fb.segspan : fb.segspan;
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
