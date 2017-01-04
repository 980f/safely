#ifndef SAFELY_H
#define SAFELY_H

/**
 * each project using the 'safely' lib can put its compile time preferences in this file,
 * code in this library that needs compiletime constants includes this header file.
 */

//compiler specific way to make 'missing break' warnings go away.
//#define JOIN
#define JOIN  [[clang::fallthrough]];

/// maximum number of values that 'Settings' protocol can handle.
#define MAXARGS 15
#define ArgsPerMessage 15

///was 'asciiframing.h'
#define FS  ','
#define EOL 10

/** AKA -1 depending upon how you look at it. We use ~0 as a marker that the -1 it hides is not a 'subtract 1' */

/** unsigned is used for all index operations.
 * Typically the only negative index value is a marker, -1.
 * It happens that if you view -1 as an unsigned it is the maximum possible value. That has the advantage of replaceing the signed integer formula:
 * index>=0 && index < quantity with a simple index<quantity, less runtime code. The only risk here is the use of -1 as a quantity value indicating that there is not even a container to have a quantity of items in. So, when returnign a quanity return 0 for 'not a valid question', that will almost always yield the expected behavior.
*/
constexpr unsigned BadIndex=~0U;
//marker for bad difference of indexes:
constexpr unsigned BadLength=~0U;

/** we default indexes to ~0, and return that as 'invalid index'*/
inline bool isValid(unsigned index){
  return index!=BadIndex;
}

#endif // SAFELY_H
