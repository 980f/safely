#ifndef STOREDENUM_H
#define STOREDENUM_H

#include "stored.h"
class StoredEnum : public Stored {
public:
  StoredEnum(Storable &node, const Enumerated &enumerated, unsigned def);
  operator unsigned() const;
  /** mimic storedInt, might actually derive from it since most of this class has been pushed into Storable.*/
  unsigned native() const;
  unsigned setto(unsigned newnum);//for when operator= syntax is just too gnarly
  unsigned operator = (unsigned newnum){
    return setto(newnum);
  }

  Cstr toString();

  /** @return a functor that when invoked will set this object's value to what is passed at this time.*/
  SimpleSlot setLater(unsigned value);
  /** a slot that will set the value of this */
  sigc::slot<void, unsigned> setter();
  /** @return a functor that when invoked will return this object's value at that time.*/
  sigc::slot<unsigned> getLater();

  /** useful for sending value changes */
  SimpleSlot applyTo(sigc::slot<void,unsigned> functor);

  /** hook up to send changes to the @param given functor, and if @param kickme call that functor now*/
  sigc::connection sendChanges(sigc::slot<void,unsigned> functor,bool kickme = false);

  /** added for a semi-pathological case in physicalSignal stuff*/
//--  void reEnumerate(const Enumerated &enumerated);
}; // class StoredEnum
#endif // STOREDENUM_H
