#ifndef BLOCK_H
#define BLOCK_H "(C) Andrew L. Heilveil, 2017"

#include "index.h" //array index with concept of 'invalid'

/** basic block manager */
template<typename Content> class Block {
  Index length;
  Content *buffer;
  //not const so that we can take it
  bool owner;
public:
  /** dangerous constructor, trusts that caller knows the length of the buffer */
  Block(unsigned length, Content *buffer, bool ownit = false) :
    length(length),
    buffer(buffer),
    owner(ownit) {
    //#nada
  }

  /** creates content, deletes it when this object is deleted*/
  Block(unsigned length) : Block(length, new Content[length], true) {
    //#nada
  }

  /** creates content, deletes it when this object is deleted*/
  Block() : Block(0, nullptr, false) {
    //#nada
  }

  ~Block() {
    if (owner) {
      delete[]buffer;
    }
    buffer = nullptr;//reduce use-after-free damage, should NPE instead of random trashing.
    length = 0;
    owner = false;
  }

  /** there are two concepts that might fall under copy construction.*/
  Block(Block &) = delete;

  Block(Block &&toBeTaken) : Block() {
    take(toBeTaken);
  };

  /** @returns this after copying content of @param toBeTaken , clears toBeTaken's owner. */
  Block &take(Block &toBeTaken) {
    this->length = toBeTaken.length;
    this->buffer = toBeTaken.buffer;
    this->owner = toBeTaken.owner;
    toBeTaken.owner = false;//we own it now
    return *this;
  }

  /** index validator */
  bool contains(unsigned index) const noexcept {
    return index < length;
  }

  /** @returns pointer to @param index th item, or a nullptr that should blow up more readiliy than if we passed back garbage.
   *  Note: the Cstr class in this library does sane things with a returned null pointer */
  Content &operator[](unsigned index) const noexcept {
    if (contains(index)) {
      return buffer[index];
    }
    return NullRef(Content);//converts illegal access into an NPE.
  }

  /** only a good idea if the class has a move constructor. */
  Content &operator()(unsigned index, const Content &defawlt) const noexcept {
    if (contains(index)) {
      return buffer[index];
    }
    return defawlt;
  }

  Index stuff(unsigned index, const Content *source, unsigned incoming) const {
    if (index + incoming < length) {
      while (incoming-- > 0) {
        buffer[index++] = *source++;
      }
      return index;
    } else {
      //TBD: do we refuse or just truncate?
      return BadLength;
    }
  }

  /** access that bypasses the sanity checks of the class */
  Content *violated() const {
    return buffer;
  }

  Index quantity() const {
    return length;
  }

  /** @returns whether this block owns the allocation */
  bool isOwned() {
    return owner;
  }
}; // class Block

#endif // BLOCK_H
