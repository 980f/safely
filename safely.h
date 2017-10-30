#ifndef SAFELY_H
#define SAFELY_H

/**
 * each project using the 'safely' lib can put its compile time preferences in this file,
 * code in this library that needs compiletime constants includes this header file.
 */

//compiler specific way to make 'missing break' warnings go away.
#define JOIN
//#define JOIN  [[clang::fallthrough]];

// controls whether json escaping rules are applied to data before being passed to Storable.
#define STORJSON_DOESCAPES 0

//migrated this out into its own file, you will need to set an include path to cppext to hve this be found.
#include "index.h"
#endif // SAFELY_H
