#ifndef CHAIN_H
#define CHAIN_H

#include "safely.h"
#include <vector>  //an STL class that is dangerous for naive users (as is all of the STL)

/**
 * Safe(r) and convenient wrapper around a vector of pointers.
 * It is very suited for container managed persistence, i.e. all objects of a class can be tracked herein and removal from here results in deletion of object.
 *
 * Since all references to the content class are pointer-like this container handles polymorphic sets of classes with ease. All such usages should have virtual
 *destructors.
 *
 * a const Chain is one that cannot have additions and deletions, a Chain of const items is a different thing.
 * const Chain<T> &things is a set of T's that cannot be added to or removed from
 * Chain<const T> &things is a mutable set of things that cannot be altered.
 */

template< typename T > class Chain {

protected:
  /** whether to call delete on things removed from the chain. Almost alaways true (by code instances) */
  bool isOwner;
  /** filters attempted appends, ill use can still put nulls into the chain.*/
  std::vector<T*> v;
public:
  Chain(bool isOwner = true) : isOwner(isOwner), v(0){
    //#nada
  }

  /** add @param thing to the end of this chain if it is not null.
   * @returns thing.
   *  Factories for type T call this integral with calling new T(...) */
  T *append(T *thing){
    if(thing) {//should never get nulls here, but if so don't add them to list.(helps with debug)
      v.push_back(thing);
    }
    return thing;
  }

  /** insert @param thing at 0-based @param location if it is not null.
   * @returns thing */
  T *insert(T *thing, unsigned location){
    if(thing) {
      v.insert(v.begin() + location,thing);
    }
    return thing;
  }

  /** @returns index of @param thing, doing an exact object compare, -1 if not contained */
  unsigned indexOf(const T *thing) const {
    if(thing) {
      for(unsigned i = v.size(); i-->0; ) {
        if(v[i] == thing) {
          return i;
        }
      }
    }
    return BadIndex;
  }

  /** @returns index of @param thing, doing an exact object compare, -1 if not contained */
  unsigned indexOf(const T &thing) const {
    return indexOf(&thing);
  }

  /** @return @param n th item of the chain.*/
  T *nth(unsigned n) const {
    if(n>=v.size()) {//#non-ternary for debug
      return nullptr; //invalid access
    }
    return v[n];
  }

  /** @return @param n th item of the chain.*/
  T *operator [](unsigned n) const {
    return nth(n);
  }

  /** @returns number of items in this chain. using type int rather than size_t to allow for -1 as a signalling value in other functions.*/
  unsigned quantity() const {
    return v.size();
  }

  /** @returns last item in chain,  nullptr if chain is empty. */
  T* last() const {
    if(unsigned qty = quantity()) {//if non-zero
      return v[qty - 1];
    } else {
      return nullptr;
    }
  }

  /** @returns first item in chain,  nullptr if chain is empty. */
  T* first() const {
    if(quantity()) {//if non-zero
      return v[0];
    } else {
      return nullptr;
    }
  }

  /** presizes chain for faster insertions via adding nullptr entries. This violates some of the expectations of other member functions */
  void allocate(unsigned howmany){
    if(howmany>quantity()) {
      v.resize(howmany,0);//adds null entries, generally not a desirable thing.
    }
  }

  /** @returns whether chain has an entry for @param ordinal. Entry itself might be nullptr if @see allocate was used/*/
  bool has(unsigned ordinal) const {
    return v.size()>ordinal;
  }

  /** removes @param n th item, 0 removes first in chain. @returns whether something was actually removed */
  bool removeNth(unsigned n){
    if(!has(n)) {
      return false;
    }
    if(isOwner) {
      delete v[n];
    }
    v.erase(v.begin() + n);
    return true;
  }

  /** removes @param n th item, 0 removes first in chain. @returns the item. Compared to removeNth this never deletes the object even if this wad claims ownership */
  T* takeNth(unsigned n){
    T* adoptee = nth(n);
    if(adoptee) {
      v.erase(v.begin() + n);//things like this is sufficient reason to hate the stl.
    }
    return adoptee;
  }

  bool removeLast(){
    return removeNth(quantity() - 1);
  }

  /** removes trailing entities until only @param mark remain. If mark is past end then nothing happens. */
  void clipto(unsigned mark){
    if(isOwner) {
      for(unsigned which = quantity(); which-->mark; ) {
        delete v[which];
      }
    }
    v.erase(v.begin() + mark,v.end() );
  }

  /** removes item @param thing if present. @returns whether something was actually removed*/
  bool remove(T *thing){
    return removeNth(indexOf(thing));
  }

  /** change positions of a pair of elements. @returns whether elements existed. */
  bool swap(unsigned from, unsigned to){
    if(has(from)&&has(to)) {
      T* thing = v[from];
      v[from]=v[to];
      v[to]=thing;
      return true;
    } else {
      return false;
    }
  }

  /** move item at @param from location to @param to location, shifting the items inbetween.
   * @returns whether the operation was performed, which only happens if both indexes are in the chain.
   * This is faster than remove/insert and more importantly does not delete the item as remove does.
   *  @deprecated needs test! had bug so must not have been compiled, ever? */
  bool relocate(unsigned from, unsigned to){
    if(has(from)&&has(to)) {
      int dir = to - from;//positive if moving towards end
      if(dir==0) {
        return false;
      }
      T* thing = v[from];
      T** p = v.data();//C++11 addition. If fails compilation then will have to do a loop with iterators
      //#not inlining the below (via dir=bas(dir) until after we have proven test coverage.
      if(dir<0) {//moving down
        memmove(p + to, p + to + sizeof(T*), -dir * sizeof(T*));
      } else { //moving on up
        memmove(p + to, p + to - sizeof(T*), dir * sizeof(T*));
      }
      v[to] = thing;
      return true;
    } else {
      return false;
    }
  } // relocate

  /** removes all items. */
  void clear(){
    if(isOwner) {
      for(int i = v.size(); i-->0; ) {//#explicit iteration to make it easier to debug exceptions
        delete v[i];
      }
    }
    v.clear();
  }

  /** destruction removes all items */
  ~Chain(){
    clear();
  }

}; // class Chain

#include "sequence.h"
/** a cheap-enough to copy java-like iteration aid for vectors of pointers, such as Chain<> is.
 * This works better than std::vector::iterator as it properly deals with items being removed during iteration.
 *
 */
template<typename T> class ChainScanner : public ::Sequence<T> {
  Chain<T> &list;
  unsigned steps;///for ordinal
public:

  ChainScanner(Chain<T> &list) : list(list),steps(0){
  }

  bool hasNext(){
    return steps<list.quantity();
  }

  T &next(void){
    return *list[steps++];
  }

  T &current(void) const {
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  unsigned ordinal() const {
    return steps;
  }

  /** move pointer back. If value is bad then pointer goes to 0!*/
  void rewind(unsigned backup = BadLength){
    if(backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

  /** removes  the item that the last call to next() gave you, adjusting iteration for its absence */
  void removeLastNext(){//remove steps-1
    if(steps) {//must have nexted at least once else segv
      list.removeNth(--steps);//decrement so we don't skip the one moving into the spot erased (versus just -1).
    }
  }

  /** removes @param which nth item, adjusting as needed the iteration for its absence */
  void remove(unsigned which){
    if(which<steps) {//if removing history
      --steps;//decrement so we don't skip the one moving into the spot erased.
    }
    list.remove(which);
  }

  //todo: detect c++14 and add a forEach

}; // class ChainScanner

/** scan a const chain, one that doesn't tolerate adds or removes */
template< typename T > class ConstChainScanner : public ::Sequence<T> {
  const Chain<T> &list;
  unsigned steps;///for ordinal
public:

  ConstChainScanner(const Chain<T> &list) :
    list(list),
    steps(0){
  }

  bool hasNext(){
    return steps<list.quantity();
  }

  T &next(void){
    return *list[steps++];
  }

  T &current(void) const {
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  unsigned ordinal() const {
    return steps;
  }

  void rewind(unsigned backup = BadLength){
    if(backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

  //todo: detect c++14 and add a forEach

}; // class ConstChainScanner

/** iterate from end to start */
template<typename T> class ChainReversed : public ::Sequence<T> {
  Chain<T> &list;
  unsigned steps;///for ordinal
public:
  void rewind(){
    steps = list.quantity();
  }

  ChainReversed( Chain<T> &list) : list(list){
    rewind();
  }

  bool hasNext() {
    return steps>0;
  }

  T &next(void){
    return *(list[--steps]);
  }

  /** will throw if you haven't called hasNext at least once before calling this*/
  T &current(void) const {
    return *(list[ordinal()]);
  }

  /** @returns the integer index of the item that next() will give you, ~0 if nothing left */
  unsigned ordinal() const {
    return steps - 1;
  }

  void remove(unsigned which){
    list.remove(which);
  }

  /** removes  the item that the last call to next() gave you, adjusting iteration for its absence */
  void removeLastNext(){//which is typically the same as current.
    remove(steps);
  }

  //todo: detect c++14 and add a forEach

}; // class ChainReversed
#endif // CHAIN_H
