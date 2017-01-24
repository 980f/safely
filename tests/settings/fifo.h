#ifndef FIFO_H
#define FIFO_H

/** trying to see if fifo class is source of Junction vs Kerrville hangs */

#include "circularbuffer.h"
struct Fifo: public CircularBuffer <char>{
  Fifo(char *storage,int sizeofstorage):CircularBuffer<char>(storage,sizeofstorage){}
  void flush(bool /*ignored*/){
    CircularBuffer<char>::flush();
  }
  char &peek(void) const {
    return CircularIndexer<char>::peek();
  }

};


#endif // FIFO_H
