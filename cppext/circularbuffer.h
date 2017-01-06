#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include "circularindexer.h"
#include "sequence.h"

/** fifo wrapper around data allocated elsewise*/

template <typename Content> class CircularBuffer : public CircularIndexer <Content>, public PeekableSequence <Content> {
private:
  bool full;
  bool empty;
  unsigned writer;
public:
  CircularBuffer(Content * allocation, unsigned sizeofAllocation): CircularIndexer <Content> (allocation, sizeofAllocation){
    flush();
  }

  void flush(void){
    CircularIndexer <Content>:: init();
    writer = 0;
    empty = true;
    full = false;
  }

  bool hasNext(void) const {
    return !empty;
  }

  bool notFull(void) const {
    return !full;
  }

  /** read and pull item, if already empty then psuedo-randomly selects an item to trash!*/
  Content&next(void){
    if(!empty) {
      Content&pulled = CircularIndexer <Content>::next();
      empty = (this->pointer == writer);
      full = false;
      return pulled;
    } else {
      return CircularIndexer <Content>::peek(); //caller beware!
    }
  } /* next */

  void unget(void){
    if(!full) {
      CircularIndexer <Content>::unget();
    }
  }

  /** @returns whether buffer was full before push was attempted */
  bool push(const Content&pushee){
    if(!full) {
      this->buffer[writer++] = pushee;
      if(writer >= this->length) {//a missing '=' here overran buffer after a while.
        writer = 0;
      }
      full = (writer == this->pointer);
      empty = false;
      return true;
    } else {
      return false;
    }
  } /* push */

  /** pushes @param pushee into buffer, first removing 'next' one if already full. @returns this*/
  CircularBuffer<Content> roll(const Content&pushee){
    if(full){
      next();
    }
    push(pushee);
    return *this;
  }

  virtual bool hasPrevious(void) const {
    return false; //todo:2 try to implement this
  }

  Content&peek(void){ //QT+mingw had no problem with a ';' here, gcc arm however wouldn't create the function.
    return CircularIndexer <Content>::peek();
  }

  Content&previous(void){
    return CircularIndexer <Content>::previous();
  }
};

#endif // CIRCULARBUFFER_H
