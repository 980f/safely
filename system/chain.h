#ifndef CHAIN_H
#define CHAIN_H

//relocated sorting logic to a wrapping view. did this when the first use found a bug making a recompile of all chain users necessary.

#include <vector>
#include <string.h> //for sorter support. used with C++11 stuff.
/** extend-by-wrapping.*/
template< typename T > class ChainWrapper;
/**
Safe(r) and convenient wrapper around a vector of pointers.
*/
template< typename T > class Chain {
  friend class ChainWrapper< T >;

protected:
  /** @returns whether to call delete on things removed from the chain.*/
  bool isOwner;
  /** filters attempted appends, ill use can still put nulls into the chain.*/
  std::vector<T*> v;
public:
  Chain(bool isOwner=true):isOwner(isOwner), v(0) {
    //#nada
  }

  /** add @param thing to the end of this chain if it is not null. @return thing */
  T *append(T *thing) {
    if(thing){//should never get nulls here, but if so don't add them to list.(helps with debug)
      v.push_back(thing);
    }
    return thing;
  }

  /** insert @param thing at 0-based @param location if it is not null. @return thing */
  T *insert(T *thing,int location) {
    if(thing){
      v.insert(v.begin()+location,thing);
    }
    return thing;
  }

  /** @returns index of @param thing, doing an exact object compare, -1 if not contained */
  int indexOf(const T *thing) const {
    if(thing){
      for(int i=v.size();i-->0;){
        if(v[i] == thing){
          return i;
        }
      }
    }
    return -1;
  }

  /** @returns index of @param thing, doing an exact object compare, -1 if not contained */
  int indexOf(const T &thing) const {
    return indexOf(&thing);
  }

//  //if sigc stuff is already in scope then add some functionality, but don't directly include sigc so that we can use this class on MCU's
//#ifdef SIGCUSER_H
//  /** @returns index of highest ordinal whose element yields a true when @param filter is called upon it, ~0 if no element meets the filter.
//  the @param filter function is usually created by binding attributes of the thing to be inserted into a function whose T* operand is an existing member of the list.
//*/
//  int lastIndex(sigc::slot<bool,const T*>filter){
//    for(int i=v.size();i-->0;){
//      if(filter(v[i])){
//        return i;
//      }
//    }
//    return ~0;
//  }
//#endif

  /** @return @param n th item of the chain.*/
  T *operator [](int n) {
    if(n<0 ||n>=int(v.size())){
      return nullptr;
    }
    return v[n];
  }

  /** @return @param n th item of the chain.*/
  const T *operator [](int n) const {
    if(n<0 ||n>=int(v.size())){
      return nullptr;
    }
    return v[n];
  }

  /** @returns number of items in this chain. using type int rather than size_t to allow for -1 as a signalling value in other functions.*/
  int quantity() const {
    return v.size();
  }

  /** @returns last item in chain,  nullptr if chain is empty. */
  T* last(){
    if(int qty=quantity()){//if non-zero
      return v[qty-1];
    } else {
      return nullptr;
    }
  }

  /** @returns last item in chain,  nullptr if chain is empty. */
  const T* last()const {
    if(int qty=quantity()){//if non-zero
      return v[qty-1];
    } else {
      return nullptr;
    }
  }

  /** @returns first item in chain,  nullptr if chain is empty. */
  T* first(){
    if(quantity()){//if non-zero
      return v[0];
    } else {
      return nullptr;
    }
  }

  /** @returns first item in chain,  nullptr if chain is empty. */
  const T* first()const {
    if(quantity()){//if non-zero
      return v[0];
    } else {
      return nullptr;
    }
  }

  /** presizes chain for faster insertions via adding nullptr entries. This violates some of the expectations of other member functions */
  void allocate(int howmany){
    if(howmany>quantity()){
      v.resize(howmany,0);//adds null entries, generally not a desirable thing.
    }
  }

  /** @returns whether chain has an entry for @param ordinal. Entry itself might be nullptr if @see allocate was used/*/
  bool has(int ordinal) const {
    return int(v.size())>ordinal && ordinal>=0;
  }

  /** removes @param ordinal th item, 0 removes first in chain. @returns whether something was actually removed */
  bool removeNth(int n) {
    if(n<0 ||n>=int(v.size())){
      return false;
    }
    if(isOwner){
      delete v[n];//NEW@ calls to append()
    }
    v.erase(v.begin()+n);
    return true;
  }

  /** removes item @param thing if present. @returns whether something was actually removed*/
  bool remove(T *thing) {
    if(thing){//null guard added for easy removeChild in storable.
      return removeNth(indexOf(thing));
    }
    return false;
  }

  /** move item at @param from location to @param to location, shifting the items inbetween.
   * @returns whether the operation was performed, which only happens if both indexes are in the chain.
   *This is faster than remove/insert and more importantly does not delete the item as remove does.*/
  void relocate(int from, int to){
    if(has(from)&&has(to)){
      T* thing=v[from];
      T** p = v.data();//C++11 addition. If fails compilation then will have to do a loop with iterators
      int dir=to-from;//positive if moving towards end
      if(dir<0){//moving down
        //to to from-1 move up 1,
        memmove(p+to,p+to+sizeof(T*),-dir*sizeof(T*));
      } else if(dir==0){
        return false;
      } else {//moving on up
        memmove(p+to,p+to-sizeof(T*),dir*sizeof(T*));
      }
      v[to]=thing;
      return true;
    }
  }

  /** removes all items. */
  void clear() {
    if(isOwner){
      for(int i=v.size();i-->0;){
        delete v[i];//NEW@ calls to append
      }
    }
    v.clear();
  }

  /** destruction removes all items */
  ~Chain() {
    clear();
  }
};

#include "sequence.h"
/**
a cheap-enough to copy iteration aid for vectors of pointers, such as Chain<> is.
*/
template <typename T> class ChainScanner:public ::Sequence<T> {
  Chain<T> &list;
  int steps;///for ordinal
public:

  ChainScanner(Chain<T> &list):list(list),steps(0){
  }

  bool hasNext(){
    return steps<list.quantity();
  }

  T &next(void){
    return *list[steps++];
  }

  T &current(void){
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  int ordinal() const {
    return steps;
  }

  void rewind(unsigned int backup=0-1){
    if(backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

  void removeLastNext(){//remove steps-1
    if(steps>0){//must have nexted at least once else segv
      list.removeNth(--steps);//decrement so we don't skip the one moving into the spot erased (versus just -1).
    }
  }

  void remove(int which){
    if(which<0){
      return;
    }
    if(which<steps){//if removing history
      --steps;//decrement so we don't skip the one moving into the spot erased.
    }
    list.remove(which);
  }

};

template< typename T > class ChainWrapper{
public:
  Chain<T> &list;
  ChainWrapper(  Chain<T> &list):list(list){

  }
};

template< typename T > class ConstChainScanner : public ::Sequence< const T > {
  const Chain <T> &list;
  int steps;///for ordinal
public:

  ConstChainScanner(const Chain<T> &list):list(list),steps(0){
  }

  bool hasNext(){
    return steps<list.quantity();
  }

  const T &next(void){
    return *list[steps++];
  }

  const T &current(void){
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  int ordinal() const {
    return steps;
  }

  void rewind(unsigned int backup=0-1){
    if(backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

};

/** iterate from end to start */
template <typename T> class ChainReversed:public ::Sequence<T> {
  Chain <T> &list;
  unsigned steps;///for ordinal
public:
  void rewind(){
    steps=list.quantity();
  }

  ChainReversed( Chain<T> &list):list(list){
    rewind();
  }

  bool hasNext(){
    return steps>0;
  }

  T &next(void){
    return *(list[--steps]);
  }

  /** will throw if you haven't called hasNext at least once before calling this*/
  T &current(void){
    return *(list[ordinal()]);
  }

  /** returns -1 if nothing left */
  int ordinal() const {
    return steps-1;
  }

  void remove(int which){
    if(which<0){//coded this way for debug, want to trap removing things that don't exist.
      return;
    }
    list.remove(which);
  }

  void removeLastNext(){//which is typically the same as current.
    remove(steps);
  }

};
#endif // CHAIN_H
