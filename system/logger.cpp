#include "logger.h"
#include "eztypes.h"
#include "textkey.h"
#include "stdarg.h"

/** you must implement these two functions somewhere in your project */
extern void logmessage(const char *prefix,const char *msg,va_list &args);
extern void dumpStack(const char *prefix);

Logger::Logger(const char *location,bool enabled):prefix(location),enabled(enabled){
#if LoggerManagement == 0
  root.append(this);
#endif
}

#if LoggerManagement == 0
void Logger::listLoggers(Logger &dbg){
  Logger *scan=Logger::root.peer;
  while(scan->peer){
    scan=scan->peer;
    dbg("Logger.%s=%d",scan->prefix,scan->enabled);
  }
}
#endif

Logger::~Logger(){
#if LoggerManagement == 0
  root.remove(this);
#endif
}

void Logger::operator() (const char *msg, ...){
  if(enabled){
    va_list args;
    va_start(args, msg);
    logmessage(prefix, msg, args);
    va_end(args);
  }
}

void Logger::dumpStack(const char *prefix){
  ::dumpStack(prefix);
}

