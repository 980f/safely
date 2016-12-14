#include "cycler.h"

Cycler::Cycler(int length):value(0) {
  setLength(length);//invoke constraint logic
}

int Cycler::cycle() {
  return length;
}

void Cycler::setLength(int length) {
  if(length<=0){
    length=1;
  }
  this->length = length;
  value %= length;
}

int Cycler::operator = (int force) {
  value = force % length;
  return value;
}

int Cycler::increment(void) {
  if(++value >= length) {
    value = 0;
  }
  return value;
}

Cycler::operator unsigned(void) {
  return value;
}

Cycler::operator int(void) {//bool was preferred over unsigned until I added this, and bool autoincrements.
  return value;
}

/** @returns true once per cycle, and not until the end of the first cycle if used in a typical fashion*/
Cycler::operator bool(void) {
  return increment() == 0;
}

int Cycler::operator +(int offset) {
  if(!length) {
    return offset;
  }
  int signedMod = (value + offset) % length;
  if(signedMod < 0) {
    return signedMod + length;
  }
  return signedMod;
}

int Cycler::operator +=(int offset){
  return operator =(operator +(offset));
}

int Cycler::operator -(int offset) {
  return *this + -offset;
}

int Cycler::operator++ (void) { ///pre increment
  return increment();
}

int Cycler::operator++ (int /*dummy*/) { ///post increment
  int was = value;
  increment();
  return was;
}
