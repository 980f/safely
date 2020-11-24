#ifndef BLOCK_H
#define BLOCK_H "(C) Andrew L. Heilveil, 2017"

#include "index.h" //array index with concept of 'invalid'

/** basic block manager */
template<typename Content> class Block {
  //there are not const so that we can 'take' the block's contents.
  Index length;
  Content *buffer;
  bool owner;
public:
  /** wrap existing buffer of known size */
  Block(unsigned length, Content *buffer, bool ownit = false) :
    length(length), buffer(length ? buffer : nullptr), owner(length != 0 && ownit) {
    //#nada
  }

  /** creates content, deletes it when this object is deleted */
  Block(unsigned length) : Block(length, new Content[length], true) {
    //#nada
  }

  /** creates empty/useless instance */
  Block() : Block(0, nullptr, false) {
    //#nada
  }

  ~Block() {
    release();
  }

  void release() {
    if (owner) {
      delete[]buffer;
    }
    buffer = nullptr;//reduce use-after-free damage, should NPE instead of random trashing.
    length = 0;
    owner = false;
  }

  /** don't allow copy construction until we have a good use case.*/
  Block(const Block &sacred) {
    peek(sacred);
  }

  /** there are two operations that this might be, force the user to call named functions for each */
  Block(Block &) = delete;

  /** @returns this after copying content of @param toBeTaken , clears toBeTaken's owner. */
  Block &take(Block &toBeTaken, bool fully = false) {//default for fully is historical, should chase down users and change default to true.
    release();
    this->length = toBeTaken.length;
    this->buffer = toBeTaken.buffer;
    this->owner = toBeTaken.owner;
    toBeTaken.owner = false;//we own it now
    if (fully) {
      toBeTaken.buffer = nullptr;
      toBeTaken.length = 0;
    }
    return *this;
  }

  /** move constructor full takes the operand */
  Block(Block &&toBeTaken) : Block() {
    take(toBeTaken,true);
  };

  Block &operator = (const Block &sacred){
    peek(sacred);
  }

  /** set this block to cover the same data as the @param sacred one, but not 'take' it in any sense. */
  void peek(const Block &sacred) {
    release();
    buffer =sacred.buffer;
    length =sacred.length;
    owner =false;
  }

  /** release what is owned and take full ownership of @param movable's data */
  Block &operator = (Block &&movable) {
    release();
    take(movable,true);
  }

  Block &operator = (Block &sacred)=delete;

    /** @returns whether there is any data */
  operator bool() const {
    return length > 0 && buffer != nullptr;
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

  /** write self over some memory that better be big enough! */
  void operator>>(Content *raw) const {
    for (unsigned i = 0; i < quantity();) {//todo:M memcopy
      *raw++ = buffer[i++];
    }
  }

  /** @returns the index-th element is the index is valid, or the passed in default if it is not.
   * This is only a good idea if the class has a move constructor. */
  Content &operator()(unsigned index, const Content &defawlt) const noexcept {
    if (contains(index)) {
      return buffer[index];
    }
    return defawlt;
  }

  /** requires copy constructor or assignment && operator*/
  void fill(Content &&filler) {
    for (int i = quantity(); i-- > 0;) {
      buffer[i] = filler;
    }
  }

  /** @returns index of next spot to stuff. Will not copy in anything if it doesn't all fit. */
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
  bool isOwned() const {
    return owner;
  }

  /** forget that we have some data. Misfires if the length is BadIndex. The only way that happens is if you construct one with an existing block and BadIndex for length. */
  void clip(unsigned shorter) {
    length.depress(shorter);
  }

  /** move start and adjust length to get rid of dead stuff. Disallowed if owner */
  bool removeHead(unsigned int dead) {
    if (owner) {
      return false;
    }
    buffer += dead;
    length -= dead;
    return true;
  }
}; // class Block

#endif // BLOCK_H
