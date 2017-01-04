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
const unsigned BadLength=~0U;


#endif // SAFELY_H
