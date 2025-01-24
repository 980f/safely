#ifndef FCNTLFLAGS_H
#define FCNTLFLAGS_H

/** linux specific trick to get the O_READ and similar flags without also getting all the function definitions.
 * This is useful when your module has to pass such flags on to a module which actually uses them, but otherwise should not access the fcntl features directly.
 * Compilers are now fast enough that that shouldn't matter, but this helps purify the suggestions lists of my IDE.
 */

#ifndef _FCNTL_H
#define _FCNTL_H
#include <bits/fcntl-linux.h>
#undef _FCNTL_H
#endif

#define O_REWRITE (O_WRONLY | O_TRUNC | O_CREAT)

#endif // FCNTLFLAGS_H
