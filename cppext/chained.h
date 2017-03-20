#ifndef CHAINED_H
#define CHAINED_H

/** usage:
 *  this is only appropriate if the semantics of the class are such that
 *  an instance can only appear in one list, which is contained by another member.
 *  such instances must be new'd by the caller of append().
 *  class User : public Chained<User> {
 *    ... append(new User());
 *  }
 *
 */

//template<typename T> class Chainer;
template<typename T> class Chained {
  //  friend class Chainer<T>;
  //protected:
public://for SettingsGrouper::init
  T* peer;
protected:
  Chained(){
    peer = 0;
  }

public:
  /** @returns @param newbie after appending it to chain.*/
  T* append(T* newbie){
    Chained<T>* scan = this;
    while(scan!=newbie && scan->peer!=0) {
      scan = scan->peer;
    }
    if(scan!=newbie){
      scan->peer = newbie;
    }
    return newbie;
  }

  /** nth peer, 0th is peer of @this. @return null is there aren't n links in this Chained */
  T* nth(int which) const {
#if 0    //tail recursive version:
    return (which-->0)?peer.nth(which):peer;
#else
    Chained<T>* scan = peer; //0th element
    while(which-->0 && scan) {
      scan = scan->peer;
    }
    return scan;
#endif
  } // nth

  /** @returns @param moriturus after removing it from this list (if present).
   * Returning moriturus (latin for 'about to die') allows the decision as to whether to delete the item to be made by someone else.
 */
  virtual Chained<T>* remove(Chained<T>* moriturus){
    if(moriturus==nullptr || moriturus==this){
      return nullptr;//can't remove ourself
    }
    for(Chained<T>* scan = this; scan->peer ; scan=scan->peer){
      if(moriturus==scan->peer){
        scan->peer=moriturus->peer;
        moriturus->peer=nullptr;
        break;
      }
    }
    return moriturus;
  }

}; // class Chained

template<typename T> class ChainedAnchor:public Chained<T> {
  const bool isOwner;
public:

  ChainedAnchor(T *root, bool isOwner=true):isOwner(isOwner){
    Chained<T>::peer=root;
  }

  T* append(T* newbie){
    if(Chained<T>::peer==nullptr){
      Chained<T>::peer=newbie;
      return newbie;
    } else {
      return Chained<T>::peer->append(newbie);
    }
  }

  T* remove(T* moriturus) /*override*/{ //at one time override worked, I guess that was a gcc oops?
    if( Chained<T>::remove(moriturus)){
      if(isOwner){
        delete moriturus;
        return nullptr;//pro-forma
      } else {
        return moriturus;
      }
    } else {
      return nullptr;
    }
  }

};

#include "sequence.h"
/**
 *  only T:Chained<T> will work here.
 */
template<typename T> class Chainer : public Sequence<Chained<T>> {
  Chained<T>* cursor;
public:
  Chainer(Chained<T> *obj){
    cursor = obj;
  }

  bool hasNext() const{
    return cursor!=0;
  }

  Chained<T>& next(){
    Chained<T>* current = cursor;
    cursor = cursor->peer;
    return *current;
  }

}; // class Chainer

#endif // CHAINED_H
