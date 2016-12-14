#ifndef eztypes_h
#define eztypes_h

//platform differences manager

#include <stdint.h>  //<cstdint> not available on micro.
#include "string.h"  //for strcmp()
typedef uint8_t u8;
//old std lib stuff wants plain char *'s, grrr:
#define U8S(plaincharstar) (reinterpret_cast <const u8 *> (plaincharstar))
#define U8Z(u8star) (reinterpret_cast <const char *> (u8star))
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

//legacy macro, don't use this for new code.
#define pun(type, lvalue) (*reinterpret_cast <type *> (&(lvalue)))

//lord it would be nice if C would make a standard operator for this:
#define countof(array) (sizeof(array) / sizeof((array)[0]))

//for non-bit addressable items:
inline bool bit(int patter, unsigned bitnumber){
  return (patter & (1 << bitnumber)) != 0;
}

inline unsigned int fieldMask(int width){
  return (1 << width) - 1;
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
inline unsigned int insertField(unsigned int target, unsigned int source, unsigned int offset, unsigned int width){
  unsigned int mask = fieldMask(width);

  mask <<= offset;
  return (target & ~mask) | ((source << offset) & mask);
}

inline unsigned int extractField(unsigned int source, unsigned int offset, unsigned int width){
  unsigned int mask = fieldMask(width);

  return (source >> offset) & mask;
}

//////////// inline stringy stuff /////////////
#define ERRLOC(moretext) __FILE__ "::" moretext

// some unicode chars frequently used by us mathematical types:
#define Degree "\u00b0"
#define Sup2   "\u00B2"
//Alpha was used as a variable name in some sacred code :(
#define AlphaChar "\u03b1"
#define Beta  "\u03b2"
#define Gamma "\u03b3"
#define Delta "\u03b4"

#define Mu    "\u03bc"
#define Sigma "\u03c3"
#define DegreeC Degree "C"

//////////// frequently used constructs ////////
class Boolish {
public:
  virtual bool operator = (bool truth) = 0;
  virtual operator bool(void) = 0;
};

/** @returns whether the @param string exists and is not just a null terminator */
inline bool isTrivial(const char *string){
  return string==0 || *string==0;
}

/** instantiate one of these as a local variable at the start of a compound statement to ensure the given 'lock' bit is set to !polarity for all exit paths of that block
  */
class BitLock {
  /** the (naturally atomic) item being used as a mutex */	
  u32&locker; //for a cortex-M* mcu this is usually the bit band address of something
  /** whether 'locked' is represented by a 1 */
  u32 polarity;
  /** construction assigns to the lock bit*/
  BitLock( u32 & lockBit, u32 _polarity): locker(lockBit), polarity(_polarity){
    locker = polarity;
  }
  /** destruction assigns to opposite of original assignment, blowing away any interim assignments */
  ~BitLock (){
    locker = !polarity;
  }
};

/** ensure matched increment and decrement */
class CountedLock {
  int &counter;
public:
  CountedLock(int &counter):counter(counter){
    ++counter;
  }
  ~CountedLock(){
    --counter;
  }
  /** can reference but not alter via this class.*/
  operator int()const{
    return counter;
  }
};

/** usage: DeleteOnReturn<typeofinstance>moriturus(&instance);
for functions with multiple exits, or that might get hit with exceptions.
NB: you must name an instance else it immediately deletes after construction.
*/
template <typename Deletable>
class DeleteOnReturn {
  Deletable*something;
public:
  DeleteOnReturn (Deletable*something):something(something){

  }
  DeleteOnReturn (Deletable&something):something(&something){

  }
  Deletable &object(){
    return *something;
  }

  /** cuteness, that lets us actually use the DOR object instead of getting a warning */
  operator Deletable &(){
    return object();
  }
  operator Deletable *(){
    return something;
  }
  operator const Deletable &() const {
    return *something;
  }

  operator bool () const {
    return something!=nullptr;
  }

  ~DeleteOnReturn(){
    delete something;
  }
};

/** delete an object and zero the pointer that pointed to it.
attempts to make it a function were painful. Should try templating
*/
#define Obliterate(thingpointer) do {delete thingpointer; thingpointer=nullptr;} while(0)

//in case some other compiler is used someday, this is gcc specific:
#define PACKED __attribute__ ((packed))

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
