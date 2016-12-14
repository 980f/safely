#ifndef EVENTFLAG_H
#define EVENTFLAG_H

/** wrapper for what needs to have an atomic read and clear.*/
class EventFlag {
  bool flag;
public:
  EventFlag(void){
    flag = false;
  }

  void set(void){
    flag = true;
  }

  bool setIf(bool andterm){
    flag |= andterm;
    return flag;
  }

  operator bool(){
    if(flag) {
      flag = false;
      return true;
    }
    return false;
  }

};

#endif // EVENTFLAG_H
