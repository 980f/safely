#ifndef POSIXLOGGER_H
#define POSIXLOGGER_H

#include "logger.h"
#include "stdio.h"  //va_list
#include "nanoseconds.h"
/** implement logging to stdout or stderr */


/** trapping signals is made conditional in case the application has its own handler.*/
void PosixLoggerInit(bool trapSignals = false);
void setStamper(NanoSeconds * stamp);

void logmessage(const char *prefix, const char *msg, va_list &args, bool moretocome = false);
void dumpStack(const char *prefix);

#ifndef IgnoreGlib
/** how to record an error from glib that you are ignoring. Useful in catch phrases. */
#define IgnoreGlib(err) dbg("%s ignoring %s",__PRETTY_FUNCTION__, err.what().c_str())
#endif

#endif // POSIXLOGGER_H
