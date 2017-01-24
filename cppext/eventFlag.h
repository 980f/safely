#ifndef EVENTFLAG_H
#define EVENTFLAG_H

/** wrapper for what needs to have an atomic read and clear.
 * This is an excellent choice for "do just once regardless of how many times I tell you to do it".
*/
class EventFlag {
  bool flag;
public:
  EventFlag(bool init=false):flag(init){
    //#nada
  }

  /** read and clear */
  operator bool(){
    if(flag) {
      flag = false;
      return true;
    }
    return false;
  }

  void set(void){
    flag = true;
  }

  /** set if @param andterm is true, if already set it stays set. */
  bool operator |=(bool andterm){
    return flag |= andterm;
  }

  /** clear if @param andterm is <b>false</b>, if already set it stays set. */
  bool operator &=(bool andterm){
    return flag &= andterm;
  }

  /** unconditional set */
  void operator ++(){
    flag=true;
  }

  /** unconditional clr */
  void operator --(){
    flag=false;
  }

};

#endif // EVENTFLAG_H
