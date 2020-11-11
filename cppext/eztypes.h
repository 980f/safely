#ifndef eztypes_h
#define eztypes_h

//platform differences manager, named for repo it was first created for "980F/ezcpp"

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

//use these when you have to force a symbol to resolve before token pasting
#define MACRO_cat(a, ...) a ## __VA_ARGS__
#define MACRO_wrap(prefix, var, suffix) prefix ## var ## suffix


/** delete an object and zero the pointer that pointed to it.
 * see Free() in cheaptricks.h
 */
#define Obliterate(thingpointer) do {delete thingpointer; thingpointer = nullptr;} while(0)

//in case some other compiler is used someday, this is gcc specific:
#ifndef PACKED
#define PACKED __attribute__((packed))
#endif


#ifndef WEAK
#define WEAK __attribute((weak))
#endif


#define INLINETHIS __attribute__((always_inline))

#define ISRALIGN __attribute__((__aligned__(4)))

//function is used in an isr, should be speed optimized:
#if OptimizeSafely
#define ISRISH __attribute__((optimize(3)))
#else
#define ISRISH
#endif

//a function suitable for handling interrupts:
using Handler= void (*)(void);

#define InitStep(k) __attribute__((init_priority(k)))
//lower happens first. some dox say leave 0..101 for the compiler.
#define InitHardware 10000
#define InitUserLibrary 20000
#define InitApplication 30000

//and sometimes you just gotta do something dangerous: (if you use nullptr instead of 0 the compiler catches on!)
#define NullRef(sometype) *reinterpret_cast<sometype *>(0)
#define NullPointer(sometype) reinterpret_cast<sometype *>(0)

#endif /* ifndef eztypes_h */
