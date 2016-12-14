#ifndef TRANSACTIONALBUFFER_H
#define TRANSACTIONALBUFFER_H

#include "buffer.h"
/**
  * wraps a buffer so that if you overflow the end then everything added since this wrapper was created is logically removed.
  * usage: case: try to print, discovering there isn't enough room, delete all the incompletely printed items.
'Transactional' comes from RDBM nomenclature.
  *
  */
template <typename Content> class TransactionalBuffer : public Indexer <Content> {
  bool failed;
  unsigned int atStart;
protected:

  void doRollBack(void){
    Indexer< Content >::rewind( Indexer <Content>::used() - atStart);
  }

  void onConstruction(void){
    failed = false;
    atStart = Indexer <Content>::used();
  }
public:
  //on creation record pointer
  TransactionalBuffer(Indexer <Content> &p): Indexer <Content> (p){
    onConstruction();
  }

  TransactionalBuffer(TransactionalBuffer <Content> &other){
    onConstruction();
  }

  //on destruction if overflowed rewind pointer
  ~TransactionalBuffer(){
    commit();
  }

  bool willRollback(void) const {
    return failed;
  }

  /** @deprecated untested
    * rollback now regardless of failure status, and be "not failed"
    * this is useful for trying alternatives without commiting. */
  void rollback(void){
    doRollBack();
    failed = false;
  }

  /** @returns whether new data was successfully added.*/
  bool commit(void){
    if(failed) {
      rollback();
      return false;
    } else {
      return true;
    }
  }

  /** once failed the transaction cannot be unfailed except by a rollback */
  void setFailed(void){
    failed = true;
  }

  bool operator &= (bool ok) {
    failed |= !ok;
    return ok;
  }

  /** user must call this instead of the wrapped buffer's next or the class is in vain.*/
  Content& next(void){
    failed &= !Indexer <Content>::hasNext();
    return Indexer <Content>::next();
  } /* next */
};

#endif // TRANSACTIONALBUFFER_H
