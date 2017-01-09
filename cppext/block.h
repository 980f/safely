#ifndef BLOCK_H
#define BLOCK_H

#include <utility>

template <typename Content> struct Block {
  unsigned length;
  Content *buffer;

  Block(unsigned length,Content *buffer):length(length),buffer(buffer){}
  ~Block(){}
  //implicit copy and move operations work fine.

  /** index validator */
  bool contains(unsigned index)const noexcept{
    return index<length;
  }

  /** @returns pointer to @param index th item, or a nullptr that should blow up more readiliy than if we passed back garbage.
Note: the Cstr class in this library does sane things with a returned null pointer */
  Content & operator [](unsigned index) const noexcept{
    if(contains(index)){
      return  buffer[length];
    }
    return reinterpret_cast<Content &>(nullptr);//converts illegal access into an NPE.
  }

  /** only a good idea if the class has a move constructor. */
  Content operator ()(unsigned index,const Content &defawlt)const noexcept{
    if(contains(index)){
      return  buffer[length];
    }
    return defawlt;
  }

};

#endif // BLOCK_H
