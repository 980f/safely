#pragma once
//(C) 2020 Andrew Heilveil (980F).

//originally was inside safely/cppext/eztypes.h

/** instantiate one of these as a local variable at the start of a compound statement to ensure the given 'lock' bit is set to !polarity for all exit paths of that
 * block
 */
class BitLock {
  /** the (naturally atomic) item being used as a mutex */
  unsigned &locker; //for a cortex-M* mcu this is usually the bit band address of something
  /** whether 'locked' is represented by a 1 */
  bool polarity;
  /** construction assigns to the lock bit*/
  BitLock( unsigned &lockBit, bool _polarity) : locker(lockBit), polarity(_polarity){
    locker = polarity;
  }

  /** destruction assigns to opposite of original assignment, blowing away any interim assignments */
  ~BitLock(){
    locker = !polarity;
  }

}; // class BitLock


