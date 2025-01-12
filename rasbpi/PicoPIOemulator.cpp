/**
// Created by andyh on 1/10/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#include "PicoPIOemulator.h"
#include <bitbanger.h>
#include <bit> //rotate instructions are still new to C++, despite being present in nearly every microcomputer ever, at least since hte early 1970's when C was invented.

void PicoPIOemulator::SM::Fifo::push(unsigned value) {
  if (!isFull()) {
    mem[++writepointer] = value;
    ++quantity;
  }
  //else do we do anything? overrun?
}

unsigned PicoPIOemulator::SM::Fifo::pull() {
  if (!isEmpty()) {
    --quantity;
    return mem[readpointer++];
  }
  return mem[readpointer]; //guessing at what hardware does, caller should check isEmpty before pull'ing.
}

void PicoPIOemulator::SM::Fifo::setSize(unsigned size) {
  if (changed(memSize, size)) {
    //forget the content.
    quantity = 0;
    readpointer = writepointer = 0;
    //but we won't bother clearing the memory itself.
  }
}

unsigned PicoPIOemulator::SM::GPIO::input(BitSpan span) {
  //actually had to be a right rotate,the lsbs normally discarded go to the msbs.
  return std::rotr(pin, span.lsb) & fieldMask(span.count);
}

void PicoPIOemulator::SM::GPIO::output(BitSpan pins, unsigned value) {
  //todo: theoretically we might write to the lsbs of gpio with a numbits+lsb exceeds 32, but there are 2 not implemented bits, 30,31, making that useless.
  insertField(pin, value, pins.lsb + pins.count - 1, pins.lsb);
}

void PicoPIOemulator::SM::GPIO::outdir(BitSpan pins, unsigned value) {
  insertField(dir, value, pins.lsb + pins.count - 1, pins.lsb);
}

void PicoPIOemulator::SM::ShiftRegister::load(uint32_t value) {
  SR = value;
  consumed = 0;
}

void PicoPIOemulator::SM::ShiftRegister::input(unsigned numBits, bool fromMsb, unsigned source) {
  if (fromMsb) {
    SR >>= numBits;
    SR |= (source & fieldMask(numBits, 0)) << (size - numBits);
  } else {
    SR <<= numBits;
    SR |= source & fieldMask(numBits, 0);
  }
  consumed += numBits; //which might go over 32, but that is apparently undefined behavior.
}

unsigned PicoPIOemulator::SM::ShiftRegister::output(unsigned numBits, bool fromMsb) {
  //todo: what do we do if more bits than are present is asked for?
  unsigned target = 0;
  if (fromMsb) {
    target = SR & fieldMask(numBits, 0);
    SR >>= numBits;
  } else {
    target = SR & fieldMask(size, size - numBits);
    SR <<= numBits;
  }
  consumed += numBits; //which might go over 32, but that is apparently undefined behavior.
  return target;
}

void PicoPIOemulator::SM::sideset(unsigned opfield) {
  bool hasEnable = cfg.sideset.useEnable;
  if (hasEnable) {
    if (!bit(opfield, 4)) {
      return;
    }
  }

  if (cfg.sideset.pins.count > (hasEnable ? 4 : 5)) {
    IllegalOP(true); //and if user doesn't handle these they will get spammed with them!
    return;
  }
  int numDelayBits = 5 - hasEnable - cfg.sideset.pins.count;
  if (numDelayBits < 0) {
    IllegalOP(true);
    return;
  }

  auto fn = cfg.sideset.OEsnotPins ? &GPIO::outdir : &GPIO::output;
  (gpio.*fn)(cfg.sideset.pins, opfield >> numDelayBits);
}

bool PicoPIOemulator::SM::testCondition(Condition condition) {
  switch (condition) {
    case Always:
      return true;
    case Xzero:
      return reg.X == 0;
    case Xnzd:
      return reg.X-- > 0;
    case Yzero:
      return reg.Y == 0;
    case Ynzd:
      return reg.Y-- > 0;
    case XneY:
      return reg.X != reg.Y;
    case Pin:
      return bit(gpio.pin, cfg.jmpPin);
    case OSRne:
      return !reg.outputSR.isConsumed(); //not our job to worry about fractional last field?
    default:
      return false;
  }
}

void PicoPIOemulator::SM::JMP(Condition condition, unsigned target) {
  if (testCondition(condition)) {
    reg.PC = target;
  }
}

void PicoPIOemulator::SM::IN(unsigned source, unsigned bitCount) {
  unsigned incoming = 0;
  switch (source) {
    case 0:
      incoming = gpio.input(cfg.IN.pins);
      break;
    case 1:
      incoming = reg.X;
      break;
    case 2:
      incoming = reg.Y;
      break;
    case 3:
      incoming = 0;
      break;
    case 4:
      IllegalOP(false);
      return;
    case 5:
      IllegalOP(false);
      return;
    case 6:
      incoming = reg.inputSR.all();
      break;
    case 7:
      incoming = reg.outputSR.all();
      break;
    default:
      InternalError();
      return;
  }
  if (cfg.IN.shiftRight) {
    reg.inputSR.input(bitCount, cfg.IN.shiftRight, incoming);
  }
}

bool PicoPIOemulator::SM::PULL(bool ifEmpty, bool block) {
  if (ifEmpty) {
    // if (reg.outputSR.isConsumed()) {
    //   //todo:some kind of wait
    // }
  }

  if (block) {
    if (TX.isEmpty()) {
      return true; //do nothing but signal that a WAIT is needed.
    }

    reg.outputSR.load(reg.X);
  } else {
    reg.outputSR.load(TX.pull());
  }

  if (TX.isEmpty()) {
    if (ifEmpty) {
      //do nothing
    }
    reg.outputSR.load(reg.X);
  } else {
    reg.outputSR.load(TX.pull());
  }
  return false;
}

void PicoPIOemulator::SM::WAIT() {}

void PicoPIOemulator::SM::MOV(unsigned int target, bool reverse, bool invert, unsigned source) {
  unsigned incoming = 0;
  switch (source) {
    case 0:
      incoming = gpio.input(cfg.IN.pins); //todo: verify count source
      break;
    case 1:
      incoming = reg.X;
      break;
    case 2:
      incoming = reg.Y;
      break;
    case 3:
      incoming = 0;
      break;
    case 4:
      IllegalOP(false);
      return;
    case 5:
      incoming = cfg.ftest(RX.available(), TX.available());
      break;
    case 6:
      reg.inputSR.all();
      break;
    case 7:
      reg.outputSR.all();
      break;
    default:
      InternalError();
      return;
  }
  if (reverse && invert) {
    if (group.V1) {} else {
      IllegalOP(false);
      return;
    }
  }
  if (reverse) {
    //todo: bit reverse incoming
  }
  if (invert) {
    incoming = ~incoming;
  }
  switch (target) {
    case 0:
      gpio.output(cfg.OUT.pins, incoming); //todo: read manual
      break;
    case 1:
      reg.X = incoming;
      break;
    case 2:
      reg.Y = incoming;
      break;
    case 3:
      if (group.V1) {} else {
        IllegalOP(false);
        return;
      }
      break;
    case 4:
      inject(incoming);
    //todo: delay is not taken, but sideset is.
      break;
    case 5:
      reg.PC = incoming;
      break;
    case 6:
      reg.inputSR.load(incoming);
      break;
    case 7:
      reg.outputSR.load(incoming);
      break;
    default:
      InternalError();
      return;
  }
}

void PicoPIOemulator::SM::OUT(unsigned int opoptions, unsigned int indexCount) {
  auto pattern = reg.outputSR.output(indexCount, cfg.OUT.shiftRight);
  switch (opoptions) {
    case 0:
      gpio.output(cfg.OUT.pins, pattern);
      break;
    case 1:
      reg.X = pattern;
      break;
    case 2:
      reg.Y = pattern;
      break;
    case 3: //no op other than shifting output SR
      break;
    case 4:
      gpio.outdir(cfg.OUT.pins, pattern);
      break;
    case 5:
      reg.PC = pattern;
      break;
    case 6:
      reg.inputSR.load(pattern);
      break;
    case 7:
      inject(pattern);
      break;
    default:
      InternalError();
      return;
  }
}

void PicoPIOemulator::SM::irqWait(unsigned which, bool toBeSet) {
  while (bit(reg.irqFlags, which) != toBeSet) {
    simulatedClock();
  }
}

void PicoPIOemulator::SM::run() {
  simulating = true;
  while (simulating) {
    //first apply config
    RX.setSize(cfg.allFifosRX ? 8 : cfg.allFifosTX ? 0 : 4); //setSize does a change detect, we don't need to do that here
    TX.setSize(cfg.allFifosTX ? 8 : cfg.allFifosRX ? 0 : 4);

    if (flagged(inject.haveInjection)) { //we are supposed to clear haveInjection when the instruction is over, but doing it here works fine as we don't inspect it again until the instruction is over.
      //more on timing: clearing haveInjection indicates that the host can write a new instruction without perturbing the one in progress, and that is true at this point as the IR value they write is not the same memory location as what we use for opcode inspection.
      reg.IR = inject.IR;
      //and PC is untouched
    } else {
      reg.IR = group.mem[reg.PC]; //todo: move exec and external write to exec.
      //do PC adjustments before we potentially execute a JMP or MOV to PC
      if (reg.PC == cfg.endOfLoop) {
        reg.PC = cfg.starOfLoop;
      } else {
        ++reg.PC;
      }
    }
    sideset(extractField(reg.IR, 12, 8));
    auto opoptions = extractField(reg.IR, 7, 5);
    auto indexCount = extractField(reg.IR, 4, 0);
    switch (extractField(reg.IR, 15, 13)) {
      case 0:
        JMP(static_cast<Condition>(opoptions), indexCount);
        break;
      case 1: //wait
        if (bit(reg.IR, 6)) { //an irq wait
        } else { //gpio wait
          if (bit(reg.IR, 5)) {
            indexCount += cfg.IN.pins.lsb;
            indexCount %= 32;
          }
          pinWait(indexCount, bit(reg.IR, 7));
        }
        WAIT();
        break;
      case 2: //in
        IN(opoptions, indexCount);
        break;
      case 3: //out
        OUT(opoptions, indexCount);
        break;
      case 4:
        if (opoptions & 3 == 0) {
          IllegalOP(false); //V1: RX as mailboxes
        } else {
          if (bit(reg.IR, 7)) {
            if (PULL(bit(reg.IR, 6), bit(reg.IR, 5))) {}
          } else {
            PUSH();
          }
        }
        break;
      case 5:
        MOV(opoptions, bit(reg.IR, 4), bit(reg.IR, 3), indexCount & 7);
        break;
      case 6: {
        //the following could be done in one hairy conditional, but that was hard to read and explain so we do it in baby steps
        unsigned irqnum = indexCount & 3; //starting with the 2 lsbs
        if (bit(indexCount, 4)) { //if the msb of the field is set
          irqnum += ownIndex; //then add ownIndex
          irqnum %= 4; //modulo 4 (compiler knows to and with 3)
        }
        irqnum |= bit(indexCount, 2) << 2; //bit 2 is always used as is.
        if (bit(reg.IR, 6)) {
          clearBit(reg.irqFlags, irqnum);
          //and maybe wait for it to clear
        } else { //just clear and proceed
          setBit(reg.irqFlags, irqnum);
          if (bit(reg.IR, 7)) {
            irqWait(irqnum, false);
          }
          //and do the above wait before we start the delay!
        }
      }
      break;
      case 7:
        SET();
        break;
      default:
        InternalError();
        break;
    }
    //and now we do delay = timed wait
  }
}

PicoPIOemulator::FifoStatus::operator unsigned int() {
  image = 0;
  for (unsigned i = SMsize; i-- > 0;) {
    SM &sm = group.sm[i];
    image |= sm.RX.isFull() << (i + 0);
    image |= sm.RX.isEmpty() << (i + 8);
    image |= sm.TX.isFull() << (i + 16);
    image |= sm.TX.isEmpty() << (i + 24);
  }
  return image;
}
