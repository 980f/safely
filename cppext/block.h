#ifndef BLOCK_H
#define BLOCK_H "(C) Andrew L. Heilveil, 2017"

/** basic block manager */
template<typename Content> class Block {
  unsigned length;
  Content *buffer;
  bool owner;
public:
  /** dangerous constructor, trusts that caller knows the length of the buffer */
  Block(unsigned length,Content *buffer,bool ownit = false):
    length(length),
    buffer(buffer),
    owner(ownit){
    //#nada
  }

  /** creates content, deletes it when this object is deleted*/
  Block(unsigned length) : Block(length,new Content[length],true){
    //#nada
  }

  ~Block(){
    if(owner) {
      delete []buffer;
    }
    buffer = nullptr;//reduce use-after-free damage, should NPE instead of random trashing.
    length = 0;
    owner = 0;
  }

  /** there are two concepts that might fall under copy construction.*/
  Block(Block &) = delete;

  /** index validator */
  bool contains(unsigned index) const noexcept {
    return index<length;
  }

  /** @returns pointer to @param index th item, or a nullptr that should blow up more readiliy than if we passed back garbage.
   *  Note: the Cstr class in this library does sane things with a returned null pointer */
  Content & operator [](unsigned index) const noexcept {
    if(contains(index)) {
      return buffer[index];
    }
    return reinterpret_cast<Content &>(nullptr);//converts illegal access into an NPE.
  }

  /** only a good idea if the class has a move constructor. */
  Content& operator ()(unsigned index,const Content &defawlt) const noexcept {
    if(contains(index)) {
      return buffer[index];
    }
    return defawlt;
  }

  /** access that bypasses the sanity checks of the class */
  Content *violated(){
    return buffer;
  }

}; // class Block

#endif // BLOCK_H
