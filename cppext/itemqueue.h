#pragma once  // (C) 2020 Andrew Heilveil (github/980f). Created by andyh on 12/20/20.

/** A queue for tracking a subset of a set of items, it only allows an item to be present once.
   The set is defined by a type argument of the template.
 * There is a sentinal value of the type named 'nullish' which defaults to an instance created via the no-args constructor if no explicit value is given.
 *
 * Unless the class is an enumeration or a pointer to something this class is likely a bad choice.
 * */

template<typename Copyable, int quantity, Copyable nullish = Copyable()> class ItemQueue {
  Copyable content[quantity];

public:
  bool hasNext() const {
    return content[0] != nullish;
  }

  /** @returns a queued instance or nullish. If not nullish then the returned object has been removed from the queue */
  Copyable next() {
    auto pending = content[0];
    if (pending != nullish) {
      memcpy(&content[0], &content[1], sizeof(Copyable) * (quantity - 1));
      content[quantity - 1] = 0;
    }
    return pending;//#keep separate return for debug.
  }

  /** inserts @param entry in the first empty slot of the queue, which under normal conditions is the end.
   * 'empty' is determined by comparing the object to the 'nullish' template argument.
   * if the object is already present then the queue is not changed, an object may only be present in the queue once.
   * 'already present' is determined by an '==' compare on the entry and items in the queue.
   * @returns whether @param entry was inserted. */
  bool insert(Copyable entry) {
    for (unsigned qi = 0; qi < quantity; ++qi) {
      auto item = content[qi];
      if (item == entry) {//if same object
        //already in queue
        return true;//we can do this as all of our messages are statically allocated, none are dynamically allocated.
      }
      if (item == nullish) {
        content[qi] = entry;
        return true;
      }
    }
    return false;
  }

  /** call at startup if you aren't sure that zero init is the same as 'nullish' */
  void init() {
    for (unsigned qi = 0; qi < quantity; ++qi) {
      content[qi] = nullish;
    }
  }

////** some probably ill-conceived convenience functions:*/

  operator Copyable() {
    return next();
  }

  operator bool() const {
    return hasNext();
  }

  bool operator<<(Copyable entry) {
    return insert(entry);
  }

  bool operator>>(Copyable &entry) {
    entry = next();
  }
};
