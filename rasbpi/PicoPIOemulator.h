#pragma once
/**
// Created by andyh on 1/10/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once
#include <dout.h>  //GPIO pins?

class PicoPIOemulator {
public:
  bool V1 = false; //allow dynamic switching of architecture.
  uint16_t mem[32];

  //tokens for memory mapped access by host
  enum class SM_CBfield { //~6 words for each SM
    CLKDIV = 0, EXECCTRL, SHIFTCTRL, ADDR, INSTR, PINCTRL,
    BlockSize
  };

  class SM {
    friend class PicoPIOemulator;
    PicoPIOemulator &group;
    unsigned ownIndex = ~0; //blow hard if not explicitly initialized
    unsigned long clockCounter = 0; //for cycle tiem estimates

    SM(PicoPIOemulator *group, unsigned yourIndex) : group{*group}, ownIndex{yourIndex}, reg(cfg) {}

    struct Fifo {
      unsigned mem[8]; //alloate max
      unsigned memSize = 4; //defaults to 4
      unsigned readpointer = 0;
      unsigned writepointer = 0;
      unsigned quantity = 0;

      bool isFull() {
        return quantity == memSize;
      }

      bool isEmpty() {
        return quantity == 0;
      }

      void push(unsigned value);

      unsigned pull();

      unsigned available() {
        return quantity;
      }

      void setSize(unsigned size);
    } RX, TX;

    /** driver can push instructions at the device. This is checked at the start of every instruction execution.*/
    struct InstructionInjector {
      uint16_t IR = 0;
      bool haveInjection = false;

      /** record the instruction and set a flag to indicate it overrides the instruction memory. */
      void operator()(uint16_t opcode) {
        //todo: mutex here, like java synchronized
        haveInjection = true;
        IR = opcode;
      }
    } inject;

    struct BitSpan {
      unsigned lsb;
      unsigned count;
    };

    struct GPIO {
      unsigned pin = 0;
      unsigned dir = 0;

      unsigned input(BitSpan span);

      void output(BitSpan pins, unsigned value);

      void outdir(BitSpan pins, unsigned value);
    } gpio;

    /** for our first cut at it we are not worrying about the packing into control register words */
    struct Config {
      bool stalled = 0; //unclear whether this is control or status.

      unsigned jmpPin = 0;

      struct FstatTest {
        unsigned threshold = 0;
        bool testRx = 0; //else tx
        unsigned operator()(unsigned rx, unsigned tx) {
          return (testRx ? rx : tx) < threshold ? 0 : ~0; //RTFM
        }
      } ftest;

      unsigned starOfLoop = 0;
      unsigned endOfLoop = 31;

      bool outIsLatched = 0;
      bool inIsLatched = 0;
      unsigned OEpin = 0;

      struct SideSet {
        bool OEsnotPins = false;
        bool useEnable = false;
        BitSpan pins;
      } sideset;

      struct ShiftControl {
        unsigned threshold = 0;
        bool autoRefresh = false; //auto pull on OSR going empty
        bool shiftRight = false; //todo: check manual for reset state
        BitSpan pins;
      };

      ShiftControl IN;
      ShiftControl OUT;

      bool allFifosTX = false;
      bool allFifosRX = false;

      BitSpan set;

      unsigned nyi = ~0; //for things we don't yet know.
    } cfg;

    class ShiftRegister {
      uint32_t SR = 0;
      const int size = std::numeric_limits<decltype(SR)>::digits;
      unsigned consumed = 0;
      Config::ShiftControl &cfg;

    public:
      explicit ShiftRegister(Config::ShiftControl &cfg) : cfg{cfg} {}

      /** load all bits and clear cumulative shift count */
      void load(uint32_t value);

      /** @returns whether at least @param numBits has been shifted since the last load.
       * The user can confuse themselves by having the threshold not be a multiple of the bit quantities being moved by input/output.
       */
      bool isConsumed() const {
        return consumed >= cfg.threshold;
      }

      /** modify SR with incoming bit field */
      void input(unsigned numBits, bool fromMsb, unsigned source);

      /** return shifted out stuff */
      unsigned output(unsigned numBits, bool fromMsb);

      unsigned all() {
        return SR;
      }
    };

    struct Registers {
      //the following are definitley unsigned, a signed int will not work!
      unsigned X = 0;
      unsigned Y = 0;
      ShiftRegister inputSR;
      ShiftRegister outputSR;
      unsigned PC = 0;
      unsigned IR = 0; //separate from what pico writes to so that we can use two actual threads for emulation and get real realtime troubles.
      unsigned clock = 0; //diagnostic, for us to count instruction cycles and annotate output traces
      unsigned irqFlags = 0;
      Registers(Config &cfg): inputSR{cfg.IN}, outputSR{cfg.OUT} {}
    } reg;

    /////////////////////////////
    enum Condition {
      Always = 0,
      Xzero,
      Xnzd,
      Yzero,
      Ynzd,
      XneY,
      Pin,
      OSRne,
    };


    void InternalError() {
      //emulator has goofed up!
    }

    void IllegalOP(bool dueToConfig) {
      //unimplemented opcode pattern, or otherwise officially "undefined operation"
    }

    void sideset(unsigned opfield);

    /** @returns wheter to jump, including doing the decrements on scratch registers. */
    bool testCondition(Condition condition);

    void JMP(Condition condition, unsigned target);

    void IN(unsigned source, unsigned bitCount);


    void PUSH() {}

    /** @returns whether we need to wait on !TX.isEmpty */
    bool PULL(bool ifEmpty, bool block);

    void WAIT();

    //from instruction to pins
    void SET() {}

    void MOV(unsigned int target, bool reverse, bool invert, unsigned source);

    bool simulating;

    void OUT(unsigned int opoptions, unsigned int indexCount);

    void simulatedClock() {
      //todo: yield via minimal OSwait()
      // Anything altering pins or irq or config needs to wake up this thread.
      ++clockCounter;
    }

    void irqWait(unsigned which, bool toBeSet);

    void pinWait(unsigned bitnum, bool toBeSet);

    void run();
  }; //end SM
  /////////////////////////////////////////////////////////////////////////////////////////////////
  //back to PIO itself

  SM sm[4] = {SM(this, 0), SM(this, 1), SM(this, 2), SM(this, 3)};
  static const unsigned SMsize = 4;

  enum RegisterName {
    CTRL = 0,
    FSTAT, //full and empty bits for each SM's fifos
    FDEBUG, //stalls and overflows, host can clear stall although pushing or pulling data would seem more appropriate
    FLEVEL, //fifo levels for each fifo
    TXfifo, //push/pull access to each SM's tx fifo
    RXfifo = TXfifo + 4,
    IRQ = RXfifo + 4, IRQ_FORCE,
    //gpio intereface:
    INPUT_SYNC_BYPASS, DBG_PADOUT, DBG_PADOE, DBG_CFGINFO,
    INSTR_MEM,
    SM_cb,
    INTR = SM_cb + 4 * unsigned(SM_CBfield::BlockSize),
    IRQ0_EFS,
    IRQ1_EFS = IRQ0_EFS + 3,
  };

  /** base for implementing pack/unpack from memory mapped values to easy to work with objects. */
  struct PsuedoMemory {
    PicoPIOemulator &group;
    PsuedoMemory(PicoPIOemulator *super): group{*super} {}
    uint32_t image = 0;

    virtual operator uint32_t() {
      return image;
    }

    virtual void operator =(uint32_t pattern) {
      image = pattern;
    }
  };

  /** full and empty bits of all fifos. */
  struct FifoStatus : PsuedoMemory {
    FifoStatus(PicoPIOemulator *super) : PsuedoMemory(super) {}

    operator uint32_t() override;

    void operator=(uint32_t pattern) override {
      //no specification on what a write does.
    }
  };

  FifoStatus theFSTAT();

  PsuedoMemory &operator[](unsigned index) {
    switch (index) {
      case CTRL:
        break;
    }
  }
};
