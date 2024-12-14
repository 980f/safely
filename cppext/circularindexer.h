#pragma once

/**a useful subset of a fifo, such as for memory for a moving average */
template<typename Content> class CircularIndexer {
protected:
  unsigned int pointer;
  unsigned int length; //could use size_t, but that would be painful and we won't ever get close to 2**32 with this codebase.
  Content *buffer; // '*' rather than '&' so that we can retarget via clone or snap.
public:
  //#below: truncating divide, omit attempt to have partial last element.
  CircularIndexer(Content *wrapped, unsigned int sizeofBuffer): length(sizeofBuffer / sizeof(Content)), buffer(wrapped) {
    pointer = 0;
  }

  virtual ~CircularIndexer() = default;

  /** @returns true once per cycle, call before next() and if true then next() will return item 0*/
  bool oncePerCycle() const {
    return pointer == 0;
  }

  /** @returns current and bumps pointer*/
  virtual Content &next() {
    unsigned was = pointer++;

    if (pointer >= length) {
      pointer = 0;
    }
    return buffer[was];
  }

  virtual void unget() {
    if (pointer == 0) {
      pointer = length;
    }
    --pointer;
  }

  /** you should avoid using this for anything except diagnostics*/
  Content &interalBuffer() const {
    return &buffer;
  }

  /** @return what next will return, but without moving pointer*/
  virtual Content &peek() const {
    return buffer[pointer < length ? pointer : 0];
  }


  /**@return reference to item most likely delivered by last call to next()*/
  Content &previous() {
    return buffer[(pointer == 0 ? length : pointer) - 1];
  }

  /** a variety of 'flush' */
  void init() {
    pointer = 0;
  }
};
