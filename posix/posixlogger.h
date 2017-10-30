#ifndef POSIXLOGGER_H
#define POSIXLOGGER_H

#include "logger.h"
#include "stdio.h"  //va_list

/** implement logging to stdout or stderr */


/** trapping signals is made conditional in case the application has its own handler.*/
void PosixLoggerInit(bool trapSignals = false);

void logmessage(const char *prefix,const char *msg,va_list &args);
void dumpStack(const char *prefix);

/** how to record an error from glib that you are ignoring. Useful in catch phrases. */
#define IgnoreGlib(err) dbg("%s ignoring %s",__PRETTY_FUNCTION__, err.what().c_str())


#endif // POSIXLOGGER_H
