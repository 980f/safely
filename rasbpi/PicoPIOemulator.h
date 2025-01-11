#pragma once
/**
// Created by andyh on 1/10/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once
#include <bitbanger.h>
#include <cycler.h>
#include <dout.h>
#include <halfopen.h>

#include <bit>

class PicoPIOemulator {
public:
  /** todo: ensure maximum of 29 is allowed for low and 30 for width */
  struct BitSpan : Span {
    BitSpan(unsigned low, unsigned width): Span(low, low + width) {}

    unsigned msb() {
      return highest - 1;
    };

    unsigned extractFrom(unsigned value) {
      return extractField(value, msb(), lowest);
    }

    void insertInto(unsigned &target, unsigned value) {
      mergeInto(target, value << lowest, fieldMask(msb(), lowest));
    }
  };

  struct GPIO {
    unsigned pin;
    unsigned dir;

    unsigned input(unsigned lowest) {
      //actually had to be a right rotate,the lsbs normally discarded go to the msbs. Target will mask as needed
      return std::rotr(pin, lowest);
    }

    void output(BitSpan bits, unsigned fromlsbs) {
      //todo: import bitfield stuff rather than reproducing inline.
      fromlsbs <<= bits.lowest;
    }
  } gpio;

  /** for our first cut at it we are not worrying about the packing into control register words */
  struct Config {
    unsigned fifoThreshold;
    bool fifoTestRx; //else tx
    unsigned starOfLoop;
    unsigned endOfLoop;
    bool outIsLatched;
    bool inIsLatched;
    unsigned OEpin;
    unsigned jmpPin;
    bool sideSetOEsnotPins;
    bool useSideSetEnable;
    bool stalled;
    bool autoPush;
    bool autoPull;
    bool inputShiftsRight;
    bool outpuShiftsRight;
    unsigned pushCount;
    unsigned pullCount;
    bool allFifosTX;
    bool allFifosRX;

    unsigned outputLsPin;
    unsigned setLsPin;
    unsigned sidesetLsPin;
    unsigned inputLsPin;

    unsigned numberSetPins;
    unsigned numberSideSetPins; //max 5, if sidSetORsnotPins then maxis4
    unsigned nyi; //for things we don't yet know.
  } cfg;

  struct ShiftRegister {
    unsigned SR;
    unsigned lostbits;
    Cycler cycle;
    /** modify SR with incoming value */
    bool input(BitSpan bits, bool msb, unsigned value) {
      lostbits = 0; //extract from designated end, then shift and insert given value


      return cycle.next(); //for caller to implement auto push|pull
    }

    /** return shifted out stuff */
    bool output(BitSpan bits, bool msb, unsigned &target);
  };

  struct Registers {
    unsigned X;
    unsigned Y;
    ShiftRegister inputSR;
    ShiftRegister outputSR;
    unsigned PC;
    unsigned IR;
    unsigned clock; //diagnostic, for us to count instruction cycles and annotate output traces
  } reg;

  uint16_t mem[32];

  /////////////////////////////
  enum Condition {
    Always = 0,
    Xzero,
    Xnzd,
    Yzero,
    Ynzd,
    XneY,
    Pin,
    OSRne
  };


  void InternalError() {
    //emulator has goofed up!
  }

  void IllegalOP() {
    //unimplemented opcode pattern, or otherwise officially "undefined operation"
  }

  bool testCondition(Condition condition) {
    switch (condition) {
      case Always:
        return true;
      case Xzero:
        return reg.X == 0;
      case Xnzd:
        return reg.X-- != 0;
      case Yzero:
        return reg.Y == 0;
      case Ynzd:
        return reg.Y-- != 0;
      case XneY:
        return reg.X != reg.Y;
      case Pin:
        return gpio.input(BitSpan(cfg.jmpPin, 1));
      case OSRne:
        return reg.outputSR.SR != 0; //probably should be counter, not data
      default:
        return false;
    }
  }

  void JMP(Condition condition, unsigned target) {
    if (testCondition(condition)) {
      reg.PC = target;
    }
  }

  void IN(unsigned source, unsigned bitCount) {
    unsigned incoming = 0;
    switch (source) {
      case 0:
        incoming = gpio.input(BitSpan(cfg.inputLsPin, bitCount));
        break;
    }
    if (cfg.inputShiftsRight) {
      reg.inputSR.SR = incoming;
      // if (reg.inputSR.input(cfg.inputBits, cfg.inputDirection, gpio.input(cfg.inputBits)));
    }
  }


  void PUSH() {}

  void PULL() {}

  void WAIT();

  //from register to pins
  void OUT();

  //from instruction to pins
  void SET() {}

  void MOV(unsigned int target, bool reverse, bool invert, unsigned source) {
    unsigned incoming = 0;
    switch (source) {
      case 0:
        incoming = gpio.input(cfg.inputLsPin);
        break;
      default:
        InternalError();
        return;
    }
    if (reverse) {
      //todo: bit reverse incoming
    }
    if (invert) {
      incoming = ~incoming;
    }
    switch (target) {
      case 0:
      case 1:
        reg.X = incoming;
        break;
    }
  }

  void run() {
    reg.IR = mem[reg.PC]; //todo: move exec and external write to exec.
    auto opoptions = extractField(reg.IR, 7, 5);

    auto indexCount = extractField(reg.IR, 4, 0);
    switch (extractField(reg.IR, 15, 13)) {
      case 0:
        JMP(static_cast<Condition>(opoptions), indexCount);
        break;
      case 1: //wait
        WAIT();
        break;
      case 2: //in
        IN(opoptions, indexCount);
        break;
      case 3: //out
        OUT();
        break;
      case 4:
        if (opoptions & 3 == 0) {
          IllegalOP(); //V1: RX as mailboxes
        } else {
          if (bit(reg.IR, 7)) {
            PULL();
          } else {
            PUSH();
          }
        }
        break;
      case 5:
        MOV(opoptions, bit(reg.IR, 4), bit(reg.IR, 3), indexCount & 7);
        break;
      case 6:
        break;
      case 7:
        SET();
        break;
      default:
        InternalError();
        break;
    }
    //side set, definitely after OUT executes
    //then delay
  }
};
