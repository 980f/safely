#pragma once
//(C) 2020 Andrew Heilveil (980F).

//originally was inside safely/cppext/eztypes.h

/** instantiate one of these as a local variable at the start of a compound statement to ensure the given 'lock' bit is set to !polarity for all exit paths of that block
 */
struct BitLock {
  /** the (naturally atomic) item being used as a mutex */
  bool &locker;
  /** whether 'locked' is represented by a 1 */
  const bool polarity;
  /** construction assigns to the lock bit*/
  BitLock( bool &lockBit, bool _polarity) : locker(lockBit), polarity(_polarity){
    locker = polarity;
  }

  /** destruction assigns to opposite of original assignment, blowing away any interim assignments */
  ~BitLock(){
    locker = !polarity;
  }

}; // class BitLock

