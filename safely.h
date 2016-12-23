#ifndef SAFELY_H
#define SAFELY_H

/**
 * each project using the 'safely' lib can put its compile time preferences in this file,
 * code in this library that needs compiletime constants include this header file.
 */

//#define JOIN
#define JOIN  [[clang::fallthrough]];



/// maximum number of values that 'Settings' protocol can handle.
#define MAXARGS 15
#define ArgsPerMessage 15

///was 'asciiframing.h'
#define FS  ','
#define EOL 10

#endif // SAFELY_H
