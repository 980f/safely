#pragma once

#include <forward_list>
#include <list>

#include "index.h"
#include <vector>  //an STL class that is dangerous for naive users (as is all of the STL)


/**
 * Safe(r) and convenient wrapper around a vector of pointers.
 * It is very suited for container managed persistence, i.e. all objects of a class can be tracked herein and removal from here results in deletion of object.
 *
 * Since all references to the content class are pointer-like this container handles polymorphic sets of classes with ease.
 * All such usages should have virtual destructors.
 *
 * a const Chain is one that cannot have additions and deletions, a Chain of const items is a different thing.
 * const Chain<T> &things is a set of T's that cannot be added to or removed from
 * Chain<const T> &things is a mutable set of things each of which cannot be altered.
 */

template<typename T> class Chain {
protected:
  /** whether to call delete on things removed from the chain. Almost always true (by code instances) */
  bool isOwner;
  /** filters attempted appends, ill use can still put nulls into the chain.*/
  std::vector<T *> v;
  /* an iterator like thing which handles insertions and removals on the main object by any instance of scanner, not just itself like the prior instantiation of ChainScanner */
  class Scanner;
  std::forward_list<Scanner> scanners; //cheapest list, but maybe this should be a Chain itself!
public:
  class Scanner {
    Chain &chain;
    unsigned step;
    bool inReverse;

  public:
    Scanner(Chain &chain, bool inReverse = false) : chain(chain), step(inReverse ? chain.quantity() : 0), inReverse(inReverse) {
      chain.scanners.push_front(this); //cheapest adder and we don't care what the order is.
    }

    ~Scanner() {
      chain.scanners.remove(this);
    }

    operator bool() const {
      return inReverse ? step : step < chain.quantity();
    }

    /* access next, unconditional increment of pointer.
     * NB: overloading operator* might seem to make more syntactic sense, but has the expectations of controlling the pointer motion via -- and ++
     */
    T &operator()() {
      //todo: add idiot guard for those who don't check operator bool()
      return chain.v[inReverse ? --step : step++];
    }

    /** NB: index of the next item that op() will return */
    unsigned ordinal() const {
      return step-inReverse;
    }

    /** move pointer back. If value is bad then pointer goes to 0!*/
    void rewind(unsigned backup = BadLength) {
      if (inReverse) {
        if (backup < chain.quantity() - step) { //do not swap sides!  backup may be a value that would cause a wrap.
          step += backup;
        } else {
          step = chain.quantity();
        }
        return;
      }

      if (backup <= step) {
        step -= backup;
      } else {
        step = 0;
      }
    }

    /** removes  the item that the last call to operator() gave you, adjusting iteration for its absence */
    void removePrior() {
      if (inReverse ? chain.has(step) : step) {
        chain.removeNth(step-inReverse);
      }
    }

  protected:
    //called by Chain when it is removing an entity
    void removing(unsigned which) {
      if (which < step) {
        --step;
      }
    }

    //called by Chain when it is inserting something
    void adding(unsigned which) {
      //an item inserted before the pointer doesn't get processed
      if (which < step) {
        ++step; //keep the latest item from getting processed twice
      }
    }

    //
    // bool swapping(unsigned from , unsigned to) {
    //   //if both below step then nothing to do
    //   //if both at or above step then nothing to do
    //   //if one below and the other above then the caller might want to do something to keep one from being skipped while the other gets done twice?
    // }
  };

public:
  Chain(bool isOwner = true) : isOwner(isOwner), v(0) {
    //#nada
  }

  /** add @param thing to the end of this chain if it is not null.
   * @returns thing.
   *  Factories for type T call this integral with calling new T(...) */
  T *append(T *thing) {
    if (thing) { //should never get nulls here, but if so don't add them to list.(helps with debug)
      v.push_back(thing);
    }
    return thing;
  }

  /** insert @param thing at 0-based @param location if it is not null.
   * @returns thing */
  T *insert(T *thing, Index location) {
    if (thing) {
      v.insert(v.begin() + location, thing);
      for (auto scanner: scanners) {
        scanner.adding(location);
      }
    }
    return thing;
  }

  /** @returns index of @param thing, doing an exact object compare, -1 if not contained */
  Index indexOf(const T *thing) const {
    if (thing) {
      for (Index i = v.size(); i-- > 0;) {
        if (v[i] == thing) {
          return i;
        }
      }
    }
    return BadIndex;
  }

  /** @returns index of @param thing, doing an exact object compare, -1 if not contained */
  Index indexOf(const T &thing) const {
    return indexOf(&thing);
  }

  /** @return @param n th item of the chain.*/
  T *nth(Index n) const {
    if (n >= v.size()) { //#non-ternary for debug
      return nullptr; //invalid access
    }
    return v[n];
  }

  /** @return @param n th item of the chain.*/
  T *operator [](Index n) const {
    return nth(n);
  }


  /** @returns number of items in this chain. using type unsigned int (Index) rather than size_t to allow for ~0 as a signalling value in other functions.*/
  Index quantity() const {
    return v.size();
  }

  /** @returns last item in chain, nullptr if chain is empty. */
  T *last() const {
    if (Index qty = quantity()) { //if non-zero
      return v[qty - 1];
    } else {
      return nullptr;
    }
  }

  /** @returns first item in chain,  nullptr if chain is empty. */
  T *first() const {
    if (quantity()) { //if non-zero
      return v[0];
    } else {
      return nullptr;
    }
  }

  /** presizes chain for faster insertions via adding nullptr entries.
   * size() is NOT altered, this is just a hint to the allocator */
  unsigned allocate(unsigned howmany) {
    if (howmany > quantity()) {
      v.reserve(howmany); //former use of 'resize' here violated too many expectations.
    }
    return howmany;
  }


  /** @returns whether chain has an entry for @param ordinal.
   * as of 2025 this does NOT include (pre)allocated entries */
  bool has(unsigned ordinal) const {
    return v.size() > ordinal;
  }

  /** removes @param n th item, 0 removes first in chain. @returns whether something was actually removed */
  bool removeNth(unsigned n) {
    if (!has(n)) {
      return false;
    }
    if (isOwner) {
      delete v[n];
    }
    v.erase(v.begin() + n);
    for (auto element: scanners) {
      element.removing(n);
    }
    return true;
  }

  /** removes @param n th item, 0 removes first in chain. @returns the item. Compared to removeNth this never deletes the object even if this wad claims ownership */
  T *takeNth(unsigned n) {
    T *adoptee = nth(n);
    if (adoptee) { //if nth actually existed
      v.erase(v.begin() + n); //syntax like this is sufficient reason to hate the stl.
      for (auto element: scanners) {
        element.removing(n);
      }
    }
    return adoptee;
  }

  bool removeLast() {
    return removeNth(quantity() - 1);
  }

  /** removes trailing entities until only @param mark remain. If mark is past end then nothing happens. */
  void clipto(unsigned mark) {
    if (isOwner) {
      for (unsigned which = quantity(); which-- > mark;) {
        delete v[which];
      }
    }
    v.erase(v.begin() + mark, v.end());
    for (auto element: scanners) {
      if (element.step > mark) {
        element.step = mark;
      }
    }
  }

  /** removes item @param thing if present. @returns whether something was actually removed*/
  bool remove(T *thing) {
    return removeNth(indexOf(thing));
  }

  /** change positions of a pair of elements. @returns whether elements existed. */
  bool swap(unsigned from, unsigned to) {
    if (has(from) && has(to)) {
      T *thing = v[from];
      v[from] = v[to];
      v[to] = thing;
      return true;
    } else {
      return false;
    }
  }

  /** move item at @param from location to @param to location, shifting the items inbetween.
   * @returns whether the operation was performed, which only happens if both indexes are in the chain.
   * This is faster than remove/insert and more importantly does not delete the item as remove does.
   *  @deprecated needs test! had bug so must not have been compiled, ever? */
  bool relocate(unsigned from, unsigned to) {
    if (has(from) && has(to)) {
      int dir = int(to) - int(from); //positive if moving towards end
      if (dir == 0) {
        return false; //todo:M debate whether this should be true, as a request to not move was successful.
      }
      T *thing = v[from];
      T **p = v.data(); //C++11 addition. If fails compilation then will have to do a loop with iterators
      //#not inlining the below (via dir=bas(dir) until after we have proven test coverage.
      if (dir < 0) { //moving down
        memmove(p + to, p + to + sizeof(T *), -dir * sizeof(T *));
      } else { //moving on up
        memmove(p + to, p + to - sizeof(T *), dir * sizeof(T *));
      }
      v[to] = thing;
      return true;
    } else {
      return false;
    }
  } // relocate

  /** removes all items. */
  void clear() {
    if (isOwner) {
      for (unsigned i = v.size(); i-- > 0;) { //#explicit iteration to make it easier to debug exceptions
        delete v[i];
      }
    }
    v.clear();
  }

  /** destruction removes all items */
  virtual ~Chain() {
    clear();
  }

  /** call a function that uses a member */
  template<typename... Args> void forEach(void (*user)(Args...), Args... args) {
    for (auto each: v) {
      (*user)(each, args...);
    }
  }

  /** call a member function on each member contained */
  template<typename... Args> void forEach(void (T::*user)(Args...), Args... args) {
    for (auto each: v) {
      (each.*user)(args...);
    }
  }
}; // class Chain
#if 0 //deprecating these
#include "sequence.h"
/** a cheap-enough to copy java-like iteration aid for vectors of pointers, such as Chain<> is.
 * This works better than std::vector::iterator as it properly deals with items being removed during iteration.
 *
 */
template<typename T> class ChainScanner : public ::Sequence<T> {
  Chain<T> &list;
  unsigned steps; ///for ordinal
public:
  ChainScanner(Chain<T> &list) : list(list), steps(0) {}

  bool hasNext() {
    return steps < list.quantity();
  }

  T &next() {
    return *list[steps++];
  }

  T &current() const {
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  unsigned ordinal() const {
    return steps;
  }

  /** move pointer back. If value is bad then pointer goes to 0!*/
  void rewind(unsigned backup = BadLength) {
    if (backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

  /** removes  the item that the last call to next() gave you, adjusting iteration for its absence */
  void removeLastNext() { //remove steps-1
    if (steps) { //must have nexted at least once else segv
      list.removeNth(--steps); //decrement so we don't skip the one moving into the spot erased (versus just -1).
    }
  }

  /** removes @param which nth item, adjusting as needed the iteration for its absence */
  void remove(unsigned which) {
    if (which < steps) { //if removing history
      --steps; //decrement so we don't skip the one moving into the spot erased.
    }
    list.removeNth(which);
  }

}; // class ChainScanner

/** scan a const chain, one that doesn't tolerate adds or removes */
template<typename T> class ConstChainScanner : public ::Sequence<T> {
  const Chain<T> &list;
  unsigned steps; ///for ordinal
public:
  ConstChainScanner(const Chain<T> &list) : list(list),
    steps(0) {}

  bool hasNext() {
    return steps < list.quantity();
  }

  T &next() {
    return *list[steps++];
  }

  T &current() const {
    return *list[steps];
  }

  /** NB: if called after a next() this is one past the ordinal of the item return by next() */
  unsigned ordinal() const {
    return steps;
  }

  void rewind(unsigned backup = BadLength) {
    if (backup <= steps) {
      steps -= backup;
    } else {
      steps = 0;
    }
  }

}; // class ConstChainScanner

/** iterate from end to start */
template<typename T> class ChainReversed : public ::Sequence<T> {
  Chain<T> &list;
  unsigned steps; ///for ordinal
public:
  void rewind() {
    steps = list.quantity();
  }

  ChainReversed(Chain<T> &list) : list(list) {
    rewind();
  }

  bool hasNext() const {
    return steps > 0;
  }

  T &next() {
    return *(list[--steps]);
  }

  /** will throw if you haven't called hasNext at least once before calling this*/
  T &current() const {
    return *(list[ordinal()]);
  }

  /** @returns the integer index of the item that next() will give you, ~0 if nothing left */
  unsigned ordinal() const {
    return steps - 1;
  }

  void remove(unsigned which) {
    list.removeNth(which);
  }

  /** removes  the item that the last call to next() gave you, adjusting iteration for its absence */
  void removeLastNext() { //which is typically the same as current.
    remove(steps);
  }

}; // class ChainReversed
#endif
