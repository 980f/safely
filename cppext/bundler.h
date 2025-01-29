#pragma once

/** a singly linked list class where construction adds an item to the class, destruction removes it.
If we want to mix static, auto, and new'd we will have to add an indicator member and overload operator new as well as the constructor.
It is OK to have this class be anywhere in the extension class's list as the constructor only refers to the fields that it writes to.
*/
template<class Grouped> class Bundler {
  static Grouped *list; //=nullptr;//zero init by nature
  /** a private link keeps caller from slipping in a null on us and orphaning the tail */
  Grouped *next;

public:
  Bundler(): next(list) {
    //insert into head of list, the earliest ones formed happen to be most often the last deleted, the newest one formed is likely to be the next deleted.
    list = this; //start the list
  }

  /** unlink object, this does NOT free them, freeing these calls this. */
  virtual ~Bundler() {
    if (list == this) {
      list = list->next;
    } else {
      for (Grouped *scan = list; scan; scan = scan->next) {
        if (scan->next == this) {
          scan->next = this->next;
          break;
        }
      }
    }
  }

  /** call a member function on all members of the group using some set of args */
  template<typename... Args> static void ForAll(void (Grouped::*fn)(Args...), Args... args) {
    for (Grouped *scan = list; scan; scan = scan->next) {
      (scan->*fn)(args...);
    }
  }

  // template<typename... Args>
  // using Booleaner=bool (Grouped::*)(Args...);
  /** call a member function on all members of the group using some set of args,stopping the iteration when a false is returne. */
  template<typename... Args> static void While(bool (Grouped::*fn)(Args...), Args... args) {
    for (Grouped *scan = list; scan; scan = scan->next) {
      if (!((scan->*fn)(args...))) {
        break;
      }
    }
  }

  /** for each member of the group call a function that takes a member as an argument. */
  template<typename... Args> static void ForAll(void (*fn)(Grouped &groupee, Args...), Args... args) {
    for (Grouped *scan = list; scan; scan = scan->next) {
      fn(*scan, args...);
    }
  }

  /** Only call this if all instances of the class are created by new.
 * if this bundle is the sole owner of pointers you need some means to destroy the entities and this is it. */
  static void FreeAll() {
    while (list) {
      delete list; //this has a side effect of altering list. The last entity in the list has a null next which gets assigned to list.
    }
  }
};

//you will need this with the appropriate name replacing Grouped in your Grouped implementation file:
#define BundlerList(Grouped) template<> Grouped* Bundler<Grouped>::list = nullptr
