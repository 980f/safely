#ifndef SYNCRETIC_H
#define SYNCRETIC_H

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
protected:
  Syncretic(){
    next = 0;
    if(lister == 0) {//only existing object
      lister = this;
      return;
    }
    for(K *scan = lister; scan != 0; scan = scan->next) {
      if(scan->next == 0) {
        scan->next = this;
        return;
      }
    }
  }
  
  virtual ~Syncretic(){
    if(lister == 0) {//pathological case, deleting one that didn't get listed in the first place
      return;
    }
    if(lister == this) {//last one, often nice to know and makes the for loop that follows simpler.
      lister = 0;
      return;
    }
    for(K *scan = lister; scan != 0; scan = scan->next) {
      if(scan->next == this) {
        scan->next = this->next;
        break;
      }
    }
  }
};


#endif // SYNCRETIC_H
