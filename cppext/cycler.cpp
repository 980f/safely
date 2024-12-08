#include "cycler.h"

constexpr unsigned Cycler::unwrap(int value) const {
  if (length == 0) {
    return ~0;
  }
  value %= length;
  while (value < 0) {
    value += length;
  }
  while (value > length) { //has wrapped
    value += length; //wrap it back
  }
  return value;
}

Cycler::Cycler(unsigned length): length(length), value(length ? 0 : ~0) {}


constexpr bool Cycler::contains(unsigned index) const {
  return length ? length > index : false;
}

unsigned Cycler::operator =(int force) {
  return value = unwrap(force);
}

unsigned Cycler::increment() {
  if (length > 0) {
    if (++value >= length) {
      value = 0;
    }
  }
  return value;
}

unsigned Cycler::decrement() {
  if (length > 0) {
    if (value == 0) {
      value = length;
    }
    --value;
  }
  return value;
}

/** @returns true once per cycle, and not until the end of the first cycle if used in a typical fashion */
bool Cycler::next() {
  return increment() == 0;
}

constexpr unsigned Cycler::operator +(int offset) const {
  return unwrap(value + offset);
}

unsigned Cycler::operator +=(int offset) {
  return operator =(value + offset);
}

constexpr unsigned Cycler::operator -(int offset) const {
  return unwrap(value - offset);
}

unsigned Cycler::operator++() { ///pre increment
  return increment();
}

unsigned Cycler::operator++(int /*dummy*/) { ///post increment
  unsigned was = value;
  increment();
  return was;
}

unsigned Cycler::operator--() { ///pre
  return decrement();
}

unsigned Cycler::operator--(int /*dummy*/) { // post
  unsigned was = value;
  decrement();
  return was;
}
