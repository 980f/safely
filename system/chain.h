#ifndef CHAIN_H
#define CHAIN_H

#include <vector>

/**
Safe(r) and convenient wrapper around a vector of pointers.
It is very suited for container managed persistence, i.e. all objects of a class can be tracked herein and removal from here results in deletion of object.
 *
 * Since all references to the content class are pointer-like this container handles polymorphic sets of classes with ease.
*/
template< typename T > class Chain {

protected:
  /** whether to call delete on things removed from the chain. Almost alaways true (by code instances) */
  bool isOwner;
  /** filters attempted appends, ill use can still put nulls into the chain.*/
  std::vector<T*> v;
public:
  Chain(bool isOwner=true):isOwner(isOwner), v(0) {
    //#nada
  }

  /** add @param thing to the end of this chain if it is not null.
   * @returns thing.
  Factories for type T call this integral with calling new T(...) */
  T *append(T *thing) {
    if(thing){//should never get nulls here, but if so don't add them to list.(helps with debug)
      v.push_back(thing);
    }
    return thing;
  }

  /** insert @param thing at 0-based @param location if it is not null.
   * @returns thing */
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

  /** @return @param n th item of the chain.*/
  T *nth(int n) const {
    if(n<0 ||n>=int(v.size())){
      return nullptr;
    }
    return v[n];
  }

//  /** @return @param n th item of the chain.*/
//  const T *nth(int n) const {
//    if(n<0 ||n>=int(v.size())){
//      return nullptr;
//    }
//    return v[n];
//  }

  /** @return @param n th item of the chain.*/
  T *operator [](int n) const{
    return nth(n);
  }

//  /** @return @param n th item of the chain.*/
//  const T *operator [](int n) const {
//    return nth(n);
//  }

  /** @returns number of items in this chain. using type int rather than size_t to allow for -1 as a signalling value in other functions.*/
  int quantity() const {
    return v.size();
  }

  /** @returns last item in chain,  nullptr if chain is empty. */
  T* last()const{
    if(int qty=quantity()){//if non-zero
      return v[qty-1];
    } else {
      return nullptr;
    }
  }

//  /** @returns last item in chain,  nullptr if chain is empty. */
//  const T* last()const {
//    if(int qty=quantity()){//if non-zero
//      return v[qty-1];
//    } else {
//      return nullptr;
//    }
//  }

  /** @returns first item in chain,  nullptr if chain is empty. */
  T* first()const{
    if(quantity()){//if non-zero
      return v[0];
    } else {
      return nullptr;
    }
  }

//  /** @returns first item in chain,  nullptr if chain is empty. */
//  const T* first()const {
//    if(quantity()){//if non-zero
//      return v[0];
//    } else {
//      return nullptr;
//    }
//  }

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

  /** removes @param n th item, 0 removes first in chain. @returns whether something was actually removed */
  bool removeNth(int n) {
    if(!has(n)){
      return false;
    }
    if(isOwner){
      delete v[n];
    }
    v.erase(v.begin()+n);
    return true;
  }

  /** removes item @param thing if present. @returns whether something was actually removed*/
  bool remove(T *thing) {
      return removeNth(indexOf(thing));
  }

  /** move item at @param from location to @param to location, shifting the items inbetween.
   * @returns whether the operation was performed, which only happens if both indexes are in the chain.
   *This is faster than remove/insert and more importantly does not delete the item as remove does.
@deprecated needs test! had bug so must not have been compiled, ever? */
  bool relocate(int from, int to){
    if(has(from)&&has(to)){
      int dir=to-from;//positive if moving towards end
      if(dir==0){
              return false;
            }
      T* thing=v[from];
      T** p = v.data();//C++11 addition. If fails compilation then will have to do a loop with iterators
      //#not inlining the below (via dir=bas(dir) until after we have proven test coverage.
      if(dir<0){//moving down
        memmove(p+to,p+to+sizeof(T*),-dir*sizeof(T*));
      }  else {//moving on up
        memmove(p+to,p+to-sizeof(T*),dir*sizeof(T*));
      }
      v[to]=thing;
      return true;
    } else {
      return false;
    }
  }

  /** removes all items. */
  void clear() {
    if(isOwner){
      for(int i=v.size();i-->0;){
        delete v[i];
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
a cheap-enough to copy java-like iteration aid for vectors of pointers, such as Chain<> is.
*/
template <typename T> class ChainScanner:public ::Sequence<T> {
  Chain<T> &list;
  int steps;///for ordinal
public:

  ChainScanner(Chain<T> &list):list(list),steps(0){
  }

  bool hasNext()const{
    return steps<list.quantity();
  }

  T &next(void){
    return *list[steps++];
  }

  T &current(void)const{
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  int ordinal() const {
    return steps;
  }

  /** move pointer back. If value is bad then pointer goes to 0!*/
  void rewind(unsigned int backup=0-1){//default arg '0-1' is a cheap way of saying 'max unsigned int' that works for all sizes of int.
    if(backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

  /** removes  the item that the last call to next() gave you, adjusting iteration for its absence */
  void removeLastNext(){//remove steps-1
    if(steps>0){//must have nexted at least once else segv
      list.removeNth(--steps);//decrement so we don't skip the one moving into the spot erased (versus just -1).
    }
  }

  /** removes @param which nth item, adjusting as needed the iteration for its absence */
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

/** scan a const chain, one that doesn't tolerate adds or removes */
template< typename T > class ConstChainScanner : public ::Sequence<T> {
  const Chain <T> &list;
  int steps;///for ordinal
public:

  ConstChainScanner(const Chain<T> &list):
    list(list),
    steps(0){
  }

  bool hasNext()const{
    return steps<list.quantity();
  }

  T &next(void) {
    return *list[steps++];
  }

  T &current(void)const{
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

  bool hasNext()const{
    return steps>0;
  }

  T &next(void){
    return *(list[--steps]);
  }

  /** will throw if you haven't called hasNext at least once before calling this*/
  T &current(void)const{
    return *(list[ordinal()]);
  }

  /** @returns the integer index of the item that next() will give you, -1 if nothing left */
  int ordinal() const {
    return steps-1;
  }

  void remove(int which){
    if(which<0){//coded this way for debug, want to trap removing things that don't exist.
      return;
    }
    list.remove(which);
  }


  /** removes  the item that the last call to next() gave you, adjusting iteration for its absence */
  void removeLastNext(){//which is typically the same as current.
    remove(steps);
  }

};
#endif // CHAIN_H
