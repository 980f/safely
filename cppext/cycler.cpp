#include "cycler.h"

void Cycler::unwrap(){
  value %= length;
  while(value>length){//has wrapped
    value+=length;//wrap it back
  }
}

Cycler::Cycler(unsigned length):value(0) {
  setLength(length);//invoke constraint logic
}

unsigned Cycler::cycle()const {
  return length;
}

void Cycler::setLength(unsigned length) {
  if(length<=0){
    length=1;
  }
  this->length = length;
  unwrap();
}

bool Cycler::contains(unsigned index) const{
  return this->length>index;
}

unsigned Cycler::operator =(int force) {
  value = force;
  unwrap();
  return value;
}

unsigned Cycler::increment(void) {
  if(++value >= length) {
    value = 0;
  }
  return value;
}

Cycler::operator unsigned(void)const {
  return value;
}

/** @returns true once per cycle, and not until the end of the first cycle if used in a typical fashion*/
bool Cycler::next(void) {
  return increment() == 0;
}

unsigned Cycler::operator +(int offset) const{
  if(!length) {
    return offset;
  }
  int signedMod = (value + offset) % length;
  if(signedMod < 0) {
    return signedMod + length;
  }
  return unsigned(signedMod);
}

unsigned Cycler::operator +=(int offset){
  return operator =(operator +(offset));
}

unsigned Cycler::operator -(int offset) const {
  //todo: needs validation
  return (*this + -offset)%length;
}

unsigned Cycler::operator++ (void) { ///pre increment
  return increment();
}

unsigned Cycler::operator++ (int /*dummy*/) { ///post increment
  unsigned was = value;
  increment();
  return was;
}
