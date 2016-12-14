#ifndef CHAINED_H
#define CHAINED_H
//this class is not useful, it needs removes, and options to delete the removed items.

/** usage:
  this is only appropriate if the semantics of the class are such that
  an instance can only appear in one list, which is contained by another member.
  such instances must be new'd by the caller of append().
  class User : public Chained<User> {
     ... append(new User());
  }

*/
template <typename T> class Chainer;
template <typename T> class Chained {
  friend class Chainer<T>;
protected:
public://for SettingsGrouper::init
  T* peer;
protected:
  Chained(){
    peer=0;
  }
public: //because it is also popular to contain a list as well as to be one.
  /** @return @param newbie after appending it to chain.*/
  T* append(T* newbie){
    if(peer==0){//often is the first as pairs as the  most common list
      return peer=newbie;//it is a violation of contract for newbie's peer to not be null.
    }
    T* scan=peer;
    while(scan!=newbie && scan->peer!=0){//1st clause handles a user bug.
      scan=scan->peer;
    }
    return scan->peer=newbie;
  }

  /** nth peer, 0th is peer of @this. @return null is there aren't n links in this Chained */
  T* nth(int which) const {
#if 0    //tail recursive version:	
    if(which-->0){
      return peer.nth(which);
    }
    return peer;
#else
    T* scan=peer; //0th element
    while(which-->0 && scan){
      scan=scan.peer;
    }
    return scan;
#endif
  }

  T* remove(T* moriturus){
    return null;
  }
};


#include "sequence.h"
/**
only T:Chained<T> will work here.
*/
template <typename T> class Chainer:public Sequence<T> {
  Chained<T>* cursor;
public:
  Chainer(Chained<T> *obj){
    cursor=obj;
  }
  bool hasNext()/*const*/ {
    return cursor!=0;
  }
  T& next(){
    T* current=static_cast<T*>(cursor);
    cursor=cursor->peer;
    return *current;
  }
};

#endif // CHAINED_H
