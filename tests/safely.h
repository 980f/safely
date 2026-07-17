#ifndef SAFELY_H
#define SAFELY_H

/** copy this file into your 'safely' project and alter it to suit.
 * each project using the 'safely' lib can put its compile time preferences in this file,
 * code in this library that needs compiletime constants includes this header file.
 */


// controls whether json escaping rules are applied to data before being passed to Storable.
#define STORJSON_DOESCAPES 0
#define SAFELY_NULL 1
//////////////////////////////////////
/// from here down are things that haven't yet found a home:

/////compiler specific way to make 'missing break' warnings go away.
#if __has_cpp_attribute(fallthrough)
#define JOIN  [[fallthrough]];
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


#endif // SAFELY_H
