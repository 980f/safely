#ifndef SAFELY_H
#define SAFELY_H

/**
 * each project using the 'safely' lib can put its compile time preferences in this file,
 * code in this library that needs compiletime constants includes this header file.
 */

//compiler specific way to make 'missing break' warnings go away.
#if __has_cpp_attribute(fallthrough)
#define JOIN  [[clang::fallthrough]]
#else
#define JOIN
#endif

//sometimes null pointers get past the compiler
#if defined(SAFELY_NULL)
#define ONNULLTHIS(arg) if(!this) {return arg;}
#define ONNULLREF(ref, arg) if(!&ref) {return arg;}
#else
#define ONNULLTHIS(arg)
#define ONNULLREF(ref, arg)
#endif

// controls whether json escaping rules are applied to data before being passed to Storable.
#define STORJSON_DOESCAPES 0

//migrated this out into its own file, you will need to set an include path to cppext to hve this be found.
//#include "index.h"
#endif // SAFELY_H
