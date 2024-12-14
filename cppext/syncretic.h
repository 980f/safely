#pragma once

/**
  * When all the instances of a class need to be accessed as a group then derive from this class.
  * Deriviation forces the constructor call which in turn records the objects existence.
  * While not apparently needing to be templated the template forces a separate lister
  * per using class, else all using classes would get dropped into one list. I.E. templating is needed for a static member to be specific to each class that derives from this one.
  *
  * todo: support multithreading */

template <class K> class Syncretic {
protected:
  static K *lister;
  K *next;

  Syncretic(){
    //putting new one at the head of list is cheaper than at the end.
    next = lister;
    lister = this;
  }
  
  virtual ~Syncretic(){
    if(lister == nullptr) {//pathological case, deleting one that somehow didn't get listed in the first place
      return;
    }
    if(lister == this) {//most recently constructed one, a frequent case.
      lister = this->next;
      return;
    }
    for(K *scan = lister; scan != 0; scan = scan->next) {
      if(scan->next == this) {
        scan->next = this->next;
        return;
      }
    }
  }
};
