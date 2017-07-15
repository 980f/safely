#ifndef eztypes_h
#define eztypes_h

//platform differences manager

#include <stdint.h>  //<cstdint> not available on micro.

typedef uint8_t u8;
//old std lib stuff wants plain char *'s, grrr:
#define U8S(plaincharstar) (reinterpret_cast<const u8 *>(plaincharstar))
#define U8Z(u8star) (reinterpret_cast<const char *>(u8star))

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

//type conversion for when the compiler just won't do it for you.
#define pun(type, lvalue) (*reinterpret_cast<type *>(&(lvalue)))

//lord it would be nice if C would make a standard operator for this:
#define countof(array) (sizeof(array) / sizeof((array)[0]))

/** instantiate one of these as a local variable at the start of a compound statement to ensure the given 'lock' bit is set to !polarity for all exit paths of that
 * block
 */
class BitLock {
  /** the (naturally atomic) item being used as a mutex */
  u32&locker; //for a cortex-M* mcu this is usually the bit band address of something
  /** whether 'locked' is represented by a 1 */
  u32 polarity;
  /** construction assigns to the lock bit*/
  BitLock( u32 & lockBit, u32 _polarity) : locker(lockBit), polarity(_polarity){
    locker = polarity;
  }

  /** destruction assigns to opposite of original assignment, blowing away any interim assignments */
  ~BitLock(){
    locker = !polarity;
  }

}; // class BitLock


/** delete an object and zero the pointer that pointed to it.
 *  attempts to make it a function were painful. Should try templating
 */
#define Obliterate(thingpointer) do {delete thingpointer; thingpointer = nullptr;} while(0)

//in case some other compiler is used someday, this is gcc specific:
#define PACKED __attribute__((packed))

#if OptimizeSafely && (isQCU || isPCU)
//function is used in an isr, should be speed optimized:
#define ISRISH __attribute__((optimize(3)))
#else
#define ISRISH
#endif

#define InitStep(k) __attribute__((init_priority(k)))

#if 1 //missing on windows (mingw) platform compiler, expected by cwchar
void swprintf();
void vswprintf();
#endif

#define NullRef(sometype) *reinterpret_cast<sometype *>(0)
#define NullPointer(sometype) reinterpret_cast<sometype *>(0)

#endif /* ifndef eztypes_h */
