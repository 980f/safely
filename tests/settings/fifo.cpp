#include "fifo.h"
//#include "core-atomic.h" // so that routines at different interrupt priorities can talk to each other using a fifo.

//void Fifo::incrementPointer(char *&pointer){
//    if(++pointer== end) {
//      pointer= mem;
//    }
//  }

//Fifo::Fifo(unsigned quantity, char *mem): mem(mem), quantity(quantity){
//  clear();
//  end = mem + quantity;
//}

//void Fifo::clear(){
//  reader = writer = mem;
//  count = 0;
//  clearOnAccess=false;
//}

//int Fifo::attempt_insert(char incoming){
//  if(count < quantity) {
//    *writer = incoming;
//    if(atomic_increment(count)) {
//      return -2;
//    }
//    incrementPointer(writer);
//    return 0;
//  } else {
//    return -1;
//  }
//} // Fifo::attempt_insert

//bool Fifo::insert(char incoming){
//  if(clearOnAccess){
//    clear();
//  }
//  int pushed;
//  do {
//    pushed = attempt_insert(incoming);
//  } while(pushed == -2);
//  return pushed ? false : true;
//}

//int Fifo::attempt_remove(){
//  if(count > 0) { // can't alter count until we have preserved our datum, so no test_and-decrement stuff.
//    int pulled = *reader;
//    // alter count before pointer to reduce the window for collision. (if we bail on collisions, else is moot)
//    if(atomic_decrement(count)) {
//      return -2;
//    }
//    incrementPointer(reader);
//    return pulled;
//  } else {
//    return -1;
//  }
//} // Fifo::attempt_remove

//int Fifo::remove(){
//  if(clearOnAccess){
//    clear();
//    return -1;
//  }
//  int pulled;
//  do {
//    pulled = attempt_remove();
//  } while(pulled == -2);
//  return pulled;
//}
