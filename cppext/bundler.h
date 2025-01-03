#pragma once

/** a singly linked list class where construction adds an item to the class, destruction removes it.
 Its first use was as a base class of class Grouped, and can make a list of statically created objects.
If we want to mix static, auto, and new'd we will have to add an indicator member and overload operator new as well as the constructor.
*/
template <class Grouped> class Bundler {
  static Grouped *list;//=nullptr;//zero init by nature
  /** a private link keeps caller from slipping in a null on us and orphaning the tail */
  Grouped *next;
public:
  Bundler():next(list){
    //insert into head of list
      list = this; //start the list
  }

  /** unlink object, this does NOT free them, freeing these calls this. */
  ~Bundler(){
    if(list==this){
      list=list->next;
    } else {
      for(Grouped *scan = list; scan ; scan = scan->next) {
        if(scan->next == this) {
          scan->next = this->next;
          break;
        }
      }
    }
  }

  /** call a member function on all members of the group using some set of args */
  template<typename ... Args> static void forAll( void (Grouped::*fn)( Args ... ),Args ... args){
    for(Grouped *scan = list; scan ; scan = scan->next) {
      scan->*fn(args...);
    }
  }

  /** for each member of the group call a function that takes a member as an argument. */
  template<typename ... Args> static void forAll(void (*fn)(Grouped &groupee,Args ...),Args ... args){
    for(Grouped *scan = list; scan ; scan = scan->next) {
      fn(*scan,args ...);
    }
  }

  /** Only call this if all instances of the class are created by new.
 * if this bundle is the sole owner of pointers you need some means to destroy the entities and this is it. */
  static void freeAll(){
    while(list){
      delete list;//this has a side effect of altering list.
    }
  }

};
