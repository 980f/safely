#ifndef LOCK_H
#define LOCK_H

/**
pick a locking as in mutex mechanism for this platform
*/


#ifdef __TARGET_PROCESSOR
//if compiling for rowley
#include "nvic.h"
#else
//todo: 'isLinux' include an appropriate header.
#define LOCK(todo) ;
#endif

#endif // LOCK_H
